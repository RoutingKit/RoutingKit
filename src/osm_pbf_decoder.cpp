#include <routingkit/timer.h>

#include "osm_pbf_decoder.h"
#include "buffered_asynchronous_reader.h"
#include "file_data_source.h"
#include "protobuf.h"

#include <zlib.h>
#include <stdexcept>
#include <netinet/in.h>
#include <thread>
#include <iomanip>
#include <sstream>

#include "osmpbfformat.pb.h"

// link with -lz
// link with -lprotobuf

namespace RoutingKit{

namespace{

	const uint64_t is_header_info_available_bit = 1;
	const uint64_t is_ordered_bit = 2;
	const uint64_t was_blob_read_bit = 4;
	const uint64_t was_header_read_bit = 8;

	class OsmPBFDecompressor{
	public:
		OsmPBFDecompressor():status(0){}
		OsmPBFDecompressor(std::function<unsigned long long(char*, unsigned long long)> data_source):
			status(0),
			reader(data_source, 64<<20){
		}

		unsigned long long minimum_read_size() const {
			return (64 << 20);
		}
		
		uint64_t get_status() const {
			return status;
		}

		std::function<unsigned long long(char*, unsigned long long)> get_read_function_object(){
			return [&](char*buffer, unsigned long long how_much_to_read)->unsigned long long{
				assert(how_much_to_read >= minimum_read_size());
				reader.wait_until_buffer_is_non_empty_or_all_bytes_were_read();
				if(reader.is_finished())
					return 0;


				OSM_PBF_FORMAT::CompressedBlock blob;
				for(;;){
					char*p = reader.read(4);
					if(p == nullptr)
						return 0;
					

					uint32_t header_size = ntohl(*((uint32_t*)p));
					OSM_PBF_FORMAT::CompressedBlockHeader header;

					if(!header.ParseFromArray(reader.read_or_throw(header_size), header_size))
						throw std::runtime_error("Cannot parse OSM blob header");

					uint32_t data_size = header.block_size();

					if(header.block_type() == "OSMData"){
						status |= is_header_info_available_bit | was_blob_read_bit;
						if(!blob.ParseFromArray(reader.read(data_size), data_size))
							throw std::runtime_error("Cannot parse OSM blob");					
						break;
					} else if(header.block_type() == "OSMHeader") {
						OSM_PBF_FORMAT::FileHeader header;
						if(!header.ParseFromArray(reader.read(data_size), data_size))
							throw std::runtime_error("Cannot parse OSM Header Block");	
						
						if((status & is_header_info_available_bit) != 0 && (status & was_blob_read_bit) != 0)
							throw std::runtime_error("OSM PBF file header block must preceed all blob blocks");
						if((status & is_header_info_available_bit) != 0 && (status & was_header_read_bit) != 0)
							throw std::runtime_error("OSM PBF file contains two header blocks");

						for(int i=0; i<header.parser_must_support_size(); ++i){
							if(header.parser_must_support(i) != "DenseNodes")
								throw std::runtime_error("OSM PBF feature \""+header.parser_must_support(i)+"\" is unknown");
						}
							
						bool is_ordered = false;
						for(int i=0; i<header.parse_may_exploit_size(); ++i){
							if(header.parse_may_exploit(i) == "Sort.Type_then_ID"){
								is_ordered = true;
								break;
							}
						}

						if(is_ordered)
							status = is_header_info_available_bit | is_ordered_bit | was_header_read_bit;
						else
							status = is_header_info_available_bit | was_header_read_bit;
						
					} else {
						reader.read(data_size);
						continue;
					}
				}

				if(blob.has_uncompressed_data()) {
					uint32_t blob_size = blob.uncompressed_data_size();
					if(blob_size > how_much_to_read-4){
						throw std::runtime_error("Blob is too large. it is "+std::to_string(blob_size) + " but may be at most "+std::to_string(how_much_to_read-4));
					}
					*(uint32_t*)buffer = blob_size;
					buffer += 4;
					memcpy(buffer, blob.uncompressed_data().c_str(), blob_size);
					return blob_size + 4;
				} else if(blob.has_zlib_compressed_data()) {
					uint32_t compressed_blob_size = blob.zlib_compressed_data().size();
					uint32_t uncompressed_blob_size = blob.uncompressed_data_size();
					if(uncompressed_blob_size > how_much_to_read-4){
						throw std::runtime_error("Blob is too large. it is "+std::to_string(uncompressed_blob_size) + " but may be at most "+std::to_string(how_much_to_read-4));
					}

					*(uint32_t*)buffer = uncompressed_blob_size;
					buffer += 4;

					z_stream z;
					z.next_in   = (unsigned char*) blob.zlib_compressed_data().c_str();
					z.avail_in  = compressed_blob_size;
					z.next_out  = (unsigned char*) buffer;
					z.avail_out = how_much_to_read-4;
					z.zalloc    = Z_NULL;
					z.zfree     = Z_NULL;
					z.opaque    = Z_NULL;

					if(inflateInit(&z) != Z_OK) {
						throw std::runtime_error("Failed to initialize zlib stream.");
					}
					if(inflate(&z, Z_FINISH) != Z_STREAM_END) {
						throw std::runtime_error("Failed to completely inflate zlib stream. Probably the OSM blob decompresses to something larger than reported in the header.");
					}
					if(inflateEnd(&z) != Z_OK) {
						throw std::runtime_error("Failed to cleanup zlib stream.");
					}
					if(z.total_out != uncompressed_blob_size) {
						throw std::runtime_error("OSM blob decompresses to fewer nodes than reported in the header.");
					}
					return uncompressed_blob_size + 4;
				} else {
					throw std::runtime_error("Only raw and zlib compressed OSM blobs are supported.");
				}
			
			};
		}
	private:
		uint64_t status;
		BufferedAsynchronousReader reader;
	};
}

namespace {
	void internal_read_osm_pbf(
		BufferedAsynchronousReader&reader,
		std::function<void(uint64_t osm_node_id, double latitude, double longitude, const TagMap&tags)>node_callback, 
		std::function<void(uint64_t osm_way_id, const std::vector<std::uint64_t>&osm_node_id_list, const TagMap&tags)>way_callback, 
		std::function<void(uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags)>relation_callback, 
		std::function<void(const std::string&msg)>log_message
	){
		OSM_PBF_FORMAT::Block primblock;
		TagMap tag_map;
		std::vector<OSMRelationMember>member_list;
		std::vector<uint64_t>node_list;

		while(!reader.is_finished()){
			{
				uint32_t s = reader.read_or_throw<uint32_t>();
				primblock.ParseFromArray(reader.read_or_throw(s), s);
			}
			for(int i = 0, l = primblock.group_list_size(); i < l; i++) {
				OSM_PBF_FORMAT::Group pg = primblock.group_list(i);

				double primblock_lon_offset = 0.000000001 * primblock.offset_of_longitude();
				double primblock_lat_offset = 0.000000001 * primblock.offset_of_latitude();
				double primblock_granularity = 0.000000001 * primblock.latlon_granularity();

				if(node_callback){
					for(int i = 0; i < pg.sparse_node_list_size(); ++i) {
						OSM_PBF_FORMAT::SparseNode node = pg.sparse_node_list(i);
						uint64_t id = node.osm_node_id();
					
						if(node.key_size() != node.value_size())
							throw std::runtime_error("PBF error: key and value arrays have different length");

						tag_map.build(
							node.key_size(), 
							[&](unsigned i){ return primblock.string_list().string_list(node.key(i)).c_str(); },
							[&](unsigned i){ return primblock.string_list().string_list(node.value(i)).c_str(); }
						);

						double lon = primblock_lon_offset + primblock_granularity * node.longitude();
						double lat = primblock_lat_offset + primblock_granularity * node.latitude();

						node_callback(id, lat, lon, tag_map);
					}

					if(pg.has_dense_node()) {
						OSM_PBF_FORMAT::DenseNode dn = pg.dense_node();
						uint64_t id = 0;
						double lon = 0;
						double lat = 0;


						if(dn.key_value_pairs_size() == 0){
							tag_map.clear();
							for(int i = 0; i < dn.delta_coded_osm_node_id_size(); ++i) {
								id += dn.delta_coded_osm_node_id(i);

								lon +=  primblock_lon_offset + primblock_granularity * dn.delta_coded_longitude(i);
								lat +=  primblock_lat_offset + primblock_granularity * dn.delta_coded_latitude(i);

								node_callback(id, lat, lon, tag_map);
							}
						}else{
							int key_value_pos = 0;
							
							for(int i=0; i<dn.key_value_pairs_size(); ++i)
								if(dn.key_value_pairs(i) > primblock.string_list().string_list_size())
									throw std::runtime_error("PBF error: key-value array contains invalid id");

							for(int i = 0; i < dn.delta_coded_osm_node_id_size(); ++i) {
								id += dn.delta_coded_osm_node_id(i);

								lon +=  primblock_lon_offset + primblock_granularity * dn.delta_coded_longitude(i);
								lat +=  primblock_lat_offset + primblock_granularity * dn.delta_coded_latitude(i);
							
								unsigned base_offset = key_value_pos;
								unsigned pair_count = 0;

								if(key_value_pos > dn.key_value_pairs_size())
									throw std::runtime_error("PBF error: key-value array is not terminated by 0 for node "+std::to_string(id));

								while(dn.key_value_pairs(key_value_pos) != 0){
									key_value_pos += 2;
									++pair_count;
									if(key_value_pos > dn.key_value_pairs_size())
										throw std::runtime_error("PBF error: key-value array is not terminated by 0 for node "+std::to_string(id));
								}
								++key_value_pos;

								tag_map.build(
									pair_count, 
									[&](unsigned i){ 
										return primblock.string_list().string_list(dn.key_value_pairs(base_offset + i*2)).c_str(); 
									},
									[&](unsigned i){ 
										return primblock.string_list().string_list(dn.key_value_pairs(base_offset + i*2 + 1)).c_str(); 
									}
								);

								node_callback(id, lat, lon, tag_map);
							}
						}
					}
				}

				if(way_callback){
					for(int i = 0; i < pg.way_list_size(); ++i) {
						OSM_PBF_FORMAT::Way way = pg.way_list(i);

						uint64_t id = way.osm_way_id();

						uint64_t node = 0;
						node_list.resize(way.node_list_size());
						for(int j = 0; j < way.node_list_size(); ++j){
							node += way.node_list(j);
							node_list[j] = node;
						}

						if(way.key_size() != way.value_size())
							throw std::runtime_error("PBF error: key and value arrays have different length");

						tag_map.build(
							way.key_size(), 
							[&](unsigned i){ return primblock.string_list().string_list(way.key(i)).c_str(); },
							[&](unsigned i){ return primblock.string_list().string_list(way.value(i)).c_str(); }
						);

						way_callback(id, node_list, tag_map);
					}
				}

				if(relation_callback){
					for(int i=0; i < pg.relation_list_size(); ++i){
						OSM_PBF_FORMAT::Relation rel = pg.relation_list(i);

						uint64_t id = rel.osm_relation_id();	

						uint64_t member_id = 0;
						member_list.resize(rel.delta_coded_member_id_list_size());

						if(rel.delta_coded_member_id_list_size() != rel.member_type_list_size() || rel.delta_coded_member_id_list_size() != rel.member_role_id_size())
							throw std::runtime_error("PBF error: relation member arrays have different lengths");


						for(int l = 0; l < rel.delta_coded_member_id_list_size(); ++l){
							member_id += rel.delta_coded_member_id_list(l);
							OSMIDType member_type;
							if(rel.member_type_list(l) == 0)
								member_type = OSMIDType::node;
							else if(rel.member_type_list(l) == 1)
								member_type = OSMIDType::way;
							else if(rel.member_type_list(l) == 2)
								member_type = OSMIDType::relation;
							else
								throw std::runtime_error("PBF error: Unknown relation type");
							

							member_list[i] = {member_type, member_id, primblock.string_list().string_list(rel.member_role_id(l)).c_str()};
						}

						if(rel.key_size() != rel.value_size())
							throw std::runtime_error("PBF error: key and value arrays have different length");

						tag_map.build(
							rel.key_size(), 
							[&](unsigned i){ return primblock.string_list().string_list(rel.key(i)).c_str(); },
							[&](unsigned i){ return primblock.string_list().string_list(rel.value(i)).c_str(); }
						);

						relation_callback(id, member_list, tag_map);
					}
				}
			}
		}
	}
}

void unordered_read_osm_pbf(
	const std::string&file_name,
	std::function<void(uint64_t osm_node_id, double latitude, double longitude, const TagMap&tags)>node_callback, 
	std::function<void(uint64_t osm_way_id, const std::vector<std::uint64_t>&osm_node_id_list, const TagMap&tags)>way_callback, 
	std::function<void(uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags)>relation_callback, 
	std::function<void(const std::string&msg)>log_message
){
	assert(node_callback || way_callback || relation_callback);

	FileDataSource data_source(file_name);
	OsmPBFDecompressor decompressor(data_source.get_read_function_object());
	BufferedAsynchronousReader reader(decompressor.get_read_function_object(), decompressor.minimum_read_size());
	internal_read_osm_pbf(reader, node_callback, way_callback, relation_callback, log_message);
}

void ordered_read_osm_pbf(
	const std::string&file_name,
	std::function<void(uint64_t osm_node_id, double latitude, double longitude, const TagMap&tags)>node_callback, 
	std::function<void(uint64_t osm_way_id, const std::vector<std::uint64_t>&osm_node_id_list, const TagMap&tags)>way_callback, 
	std::function<void(uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags)>relation_callback, 
	std::function<void(const std::string&msg)>log_message,
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered
){
	assert(node_callback || way_callback || relation_callback);

	FileDataSource data_source(file_name);
	OsmPBFDecompressor decompressor(data_source.get_read_function_object());
	BufferedAsynchronousReader reader(decompressor);

	if(!file_is_ordered_even_though_file_header_says_that_it_is_unordered){
		while((decompressor.get_status() & is_header_info_available_bit) == 0){
			std::this_thread::yield();
		}
	}
	
	
	if(file_is_ordered_even_though_file_header_says_that_it_is_unordered || (decompressor.get_status() & is_ordered_bit) != 0){
		internal_read_osm_pbf(reader, node_callback, way_callback, relation_callback, log_message);
	} else {
		if(node_callback){
			internal_read_osm_pbf(reader, node_callback, nullptr, nullptr, log_message);
			if(relation_callback || way_callback){
				reader = BufferedAsynchronousReader();
				decompressor = OsmPBFDecompressor();
				data_source.rewind();
				decompressor = OsmPBFDecompressor(data_source.get_read_function_object());
				reader = BufferedAsynchronousReader(decompressor);
			}
		}

		if(way_callback){
			internal_read_osm_pbf(reader, nullptr, way_callback, nullptr, log_message);
			if(relation_callback){
				reader = BufferedAsynchronousReader();
				decompressor = OsmPBFDecompressor();
				data_source.rewind();
				decompressor = OsmPBFDecompressor(data_source.get_read_function_object());
				reader = BufferedAsynchronousReader(decompressor);
			}
		}

		if(relation_callback){
			internal_read_osm_pbf(reader, nullptr, nullptr, relation_callback, log_message);
		}
	}
}


void speedtest_osm_pbf_reading(
	const std::string&pbf_file,
	std::function<void(std::string)>log_message
){
	log_message("Starting scan speedtest");
	
	uint64_t node_count = 0;
	uint64_t way_count = 0;
	uint64_t rel_count = 0;

	long long last_report = 0;

	auto produce_report = [&](){
		long long now = get_micro_time();
		if(now - last_report > 1000000){
			std::ostringstream out;
			out << "Read "<<std::setw(9)<<node_count <<" nodes, "<<std::setw(9)<<way_count<<" ways, "<<std::setw(9)<<rel_count<<" relations last second";
			log_message(out.str());
			last_report = now;
			node_count = 0;
			way_count = 0;
			rel_count = 0;
		}
	};


	unordered_read_osm_pbf(
		pbf_file, 
		[&](uint64_t osm_node_id, double lat, double lon, const TagMap&tags){
			++node_count;
			produce_report();
		},
		[&](uint64_t osm_way_id, const std::vector<std::uint64_t>&node_id_list, const TagMap&tags) {
			++way_count;
			produce_report();
		},
		[&](uint64_t osm_rel_id, const std::vector<OSMRelationMember>&member, const TagMap&tags) {
			++rel_count;
			produce_report();
		},
		log_message
	);
	log_message("Speedtest finished");
}

} // RoutingKit

