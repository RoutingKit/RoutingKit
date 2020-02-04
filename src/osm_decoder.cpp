#include <routingkit/timer.h>
#include <routingkit/osm_decoder.h>

#include "buffered_asynchronous_reader.h"
#include "file_data_source.h"
#include "protobuf.h"

#include <zlib.h>
#include <stdexcept>
#include <thread>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <atomic>
#include <string.h>

// The following include is only there to get access to ntohl. Nothing else is
// used from the networking header. If someone has a good idea of how to
// implement ntohl in a portable way that
// (a) runs on big endian machines
// (b) runs on a semi-recent MSVC
// (c) does the endian check at compile time
// then please open an issue and tell me how.
//
// Another option would be to drop support for big-endian hosts. I know of no
// recent architecture that uses big-endian. Maybe this will happen sometime in
// the future.
#ifndef ROUTING_KIT_ASSUME_LITTLE_ENDIAN
#include <netinet/in.h>
#else
// If we know, that the machine is little endian, then we can avoid all networking 
// headers by implementing ntohl ourselves.
static uint32_t ntohl(uint32_t netlong){
	return
		(netlong << 24) |
		((netlong << 16) >> 8) |
		((netlong << 8) >> 16) |
		(netlong >> 24);
}
#endif

// link with -lz

namespace RoutingKit{

namespace{

	// formally
	//
	// char*p = ...;
	// uint32_t x = *((uint32_t*)p);
	//
	// is undefined behavior. We therefore use memcpy.

	template<class T>
	void unaligned_store(char*dest, const T&val){
		memcpy(dest, (const char*)&val, sizeof(T));
	}

	template<class T>
	T unaligned_load(const char*src){
		T ret;
		memcpy((char*)&ret, src, sizeof(T));
		return ret; // NVRO
	}

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

				const char*blob_begin, *blob_end;

				for(;;){
					char*p = reader.read(4);
					if(p == nullptr)
						return 0;


					uint32_t header_size = ntohl(unaligned_load<uint32_t>(p));

					uint32_t data_size = (uint32_t)-1;


					char block_type[16] = "";

					const char*buffer = reader.read_or_throw(header_size);
					decode_protobuf_message_with_callbacks(
						buffer, buffer+header_size,
						[&](uint64_t key_id, uint64_t num){
							if(key_id == 3)
								data_size = num;
						},
						[&](uint64_t key_id, double num){},
						[&](uint64_t key_id, const char*str_begin, const char*str_end){
							if(key_id == 1){
								unsigned len = str_end - str_begin;
								if(len > sizeof(block_type)-1)
									len = sizeof(block_type)-1;
								memcpy(block_type, str_begin, len);
								block_type[len] = '\0';
							}
						}
					);

					if(data_size == (uint32_t)-1)
						throw std::runtime_error("Cannot parse OSM blob header because it is missing the data size");

					if(!strcmp(block_type, "OSMData")){
						status |= is_header_info_available_bit | was_blob_read_bit;

						blob_begin = reader.read_or_throw(data_size);
						blob_end = blob_begin + data_size;
						break;
					} else if(!strcmp(block_type, "OSMHeader")) {
						if((status & is_header_info_available_bit) != 0 && (status & was_blob_read_bit) != 0)
							throw std::runtime_error("OSM PBF file header block must preceed all blob blocks");
						if((status & is_header_info_available_bit) != 0 && (status & was_header_read_bit) != 0)
							throw std::runtime_error("OSM PBF file contains two header blocks");

						bool is_ordered = false;
						const char*buffer = reader.read_or_throw(data_size);
						decode_protobuf_message_with_callbacks(
							buffer, buffer+data_size,
							[&](uint64_t key_id, uint64_t num){},
							[&](uint64_t key_id, double num){},
							[&](uint64_t key_id, const char*str_begin, const char*str_end){
								if(key_id == 4){ // must support
									if(!std::equal(str_begin, str_end, "DenseNodes"))
										throw std::runtime_error("Required OSM PBF feature \""+std::string(str_begin, str_end)+"\" is unknown");
								}else if(key_id == 5){ // may exploit
									if(std::equal(str_begin, str_end, "Sort.Type_then_ID"))
										is_ordered = true;
								}
							}
						);
						if(is_ordered)
							status = is_header_info_available_bit | is_ordered_bit | was_header_read_bit;
						else
							status = is_header_info_available_bit | was_header_read_bit;
					} else {
						reader.read(data_size);
						continue;
					}
				}

				const char
					*uncompressed_begin = nullptr,
					*uncompressed_end = nullptr,
					*compressed_begin = nullptr,
					*compressed_end = nullptr;
				uint64_t uncompressed_data_size = (uint64_t)-1;

				decode_protobuf_message_with_callbacks(
					blob_begin, blob_end,
					[&](uint64_t key_id, uint64_t num){
						if(key_id == 2)
							uncompressed_data_size = num;
					},
					[&](uint64_t key_id, double num){},
					[&](uint64_t key_id, const char*str_begin, const char*str_end){
						if(key_id == 1){
							uncompressed_begin = str_begin;
							uncompressed_end = str_end;
						}else if(key_id == 3){
							compressed_begin = str_begin;
							compressed_end = str_end;
						}
					}
				);

				if(uncompressed_begin != nullptr && compressed_begin != nullptr)
					throw std::runtime_error("PBF error: Blob must not contain both compressed and uncompressed data");
				if(uncompressed_begin == nullptr && compressed_begin == nullptr)
					throw std::runtime_error("PBF error: Blob contains neither compressed nor uncompressed data");
				if(uncompressed_data_size == (uint64_t)-1)
					throw std::runtime_error("PBF error: Blob does not contain the size of the uncompressed data");
				if(uncompressed_begin){
					if(uncompressed_data_size > how_much_to_read-4)
						throw std::runtime_error("PBF error: Blob is too large. It is "+std::to_string(uncompressed_data_size) + " but may be at most "+std::to_string(how_much_to_read-4));
					if(uncompressed_data_size != (std::uint64_t)(uncompressed_end - uncompressed_begin))
						throw std::runtime_error("PBF error: claimed uncompressed blob size does not correspond to actual blob size");
					unaligned_store<uint32_t>(buffer, uncompressed_data_size);
					buffer += 4;
					memcpy(buffer, uncompressed_begin, uncompressed_data_size);
					return uncompressed_data_size + 4;
				}else{
					uint64_t compressed_data_size = compressed_end - compressed_begin;
					if(uncompressed_data_size > how_much_to_read-4)
						throw std::runtime_error("PBF error: Blob is too large. It is "+std::to_string(uncompressed_data_size) + " but may be at most "+std::to_string(how_much_to_read-4));

					unaligned_store<uint32_t>(buffer, uncompressed_data_size);
					buffer += 4;

					z_stream z;
					z.next_in   = (unsigned char*) compressed_begin;
					z.avail_in  = compressed_data_size;
					z.next_out  = (unsigned char*) buffer;
					z.avail_out = how_much_to_read-4;
					z.zalloc    = Z_NULL;
					z.zfree     = Z_NULL;
					z.opaque    = Z_NULL;

					if(inflateInit(&z) != Z_OK) {
						throw std::runtime_error("PBF error: Failed to initialize zlib stream.");
					}
					if(inflate(&z, Z_FINISH) != Z_STREAM_END) {
						throw std::runtime_error("PBF error: Failed to completely inflate zlib stream. Probably the OSM blob decompresses to something larger than reported in the header.");
					}
					if(inflateEnd(&z) != Z_OK) {
						throw std::runtime_error("PBF error: Failed to cleanup zlib stream.");
					}
					if(z.total_out != uncompressed_data_size) {
						throw std::runtime_error("PBF error: OSM blob decompresses to fewer bytes than reported in the header.");
					}
					return uncompressed_data_size + 4;
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
		TagMap tag_map;
		std::vector<OSMRelationMember>member_list;
		std::vector<uint64_t>node_list;

		std::vector<const char*>string_table;
		std::vector<uint32_t>key_list;
		std::vector<uint32_t>value_list;

		std::vector<std::pair<const char*, const char*>>group_list;

		for(;;){
			char*primblock_begin, *primblock_end;
			{
				char*s_ptr = reader.read(4);
				if(s_ptr == nullptr)
					break;
				uint32_t s = unaligned_load<uint32_t>(s_ptr);
				primblock_begin = reader.read_or_throw(s);
				primblock_end = primblock_begin + s;
			}

			string_table.clear();
			group_list.clear();

			uint64_t latlon_granularity = 100;
			int64_t offset_of_latitude = 0;
			int64_t offset_of_longitude = 0;

			decode_protobuf_message_with_callbacks(
				primblock_begin, primblock_end,
				[&](uint64_t key_id, uint64_t num){
					if(key_id == 17)
						latlon_granularity = num;
					else if(key_id == 19)
						offset_of_latitude = zigzag_convert_uint64_to_int64(num);
					else if(key_id == 20)
						offset_of_longitude = zigzag_convert_uint64_to_int64(num);

				},
				[&](uint64_t key_id, double num){},
				[&](uint64_t key_id, const char*str_begin, const char*str_end){
					if(key_id == 1){
						decode_protobuf_message_with_callbacks(
							str_begin, str_end,
							[&](uint64_t key_id, uint64_t num){},
							[&](uint64_t key_id, double num){},
							[&](uint64_t key_id, const char*str_begin, const char*str_end){
								// This is ok because in the protobuf format this is the place where the length of the string stands and
								// we will no longer need it.
								char*str = const_cast<char*>(str_begin-1);
								memmove(str, str_begin, str_end-str_begin);
								*const_cast<char*>(str_end-1) = '\0';
								string_table.push_back(str);
							}
						);
					}else if(key_id == 2){
						group_list.push_back({str_begin, str_end});
					}
				}
			);

			if(latlon_granularity == 0)
				throw std::runtime_error("PBF error: latlon_granularity of a block must not be zero.");

			double primblock_lon_offset = 0.000000001 * offset_of_latitude;
			double primblock_lat_offset = 0.000000001 * offset_of_longitude;
			double primblock_granularity = 0.000000001 * latlon_granularity;

			auto decode_sparse_node = [&](const char*begin, const char*end){
				uint64_t osm_node_id = (uint64_t)-1;
				double latitude = 0.0, longitude = 0.0;

				const char
					*key_begin = nullptr, *key_end = nullptr,
					*value_begin = nullptr, *value_end = nullptr;

				decode_protobuf_message_with_callbacks(
					begin, end,
					[&](uint64_t key_id, uint64_t num){
						if(key_id == 1)
							osm_node_id = num;
						else if(key_id == 19)
							latitude = primblock_lon_offset + primblock_granularity * zigzag_convert_uint64_to_int64(num);
						else if(key_id == 20)
							longitude = primblock_lat_offset + primblock_granularity * zigzag_convert_uint64_to_int64(num);
					},
					[&](uint64_t key_id, double num){},
					[&](uint64_t key_id, const char*begin, const char*end){
						if(key_id == 2){
							key_begin = begin;
							key_end = end;
						}else if(key_id == 3){
							value_begin = begin;
							value_end = end;
						}
					}
				);

				if(osm_node_id == (uint64_t)-1)
					throw std::runtime_error("PBF error: way is missing its OSM ID.");

				key_list.clear();
				value_list.clear();
				while(key_begin != key_end && value_begin != value_end){
					key_list.push_back(decode_varint_as_uint64_and_advance_first_parameter(key_begin, key_end));
					value_list.push_back(decode_varint_as_uint64_and_advance_first_parameter(value_begin, value_end));
				}
				if(key_begin != key_end || value_begin != value_end)
					throw std::runtime_error("PBF error: key and value arrays do not decode to equal length.");

				tag_map.build(
					key_list.size(),
					[&](uint64_t i){
						i = key_list[i];
						if(i > string_table.size())
							throw std::runtime_error("PBF error: key string ID is out of bounds.");
						return string_table[i];
					},
					[&](uint64_t i){
						i = value_list[i];
						if(i > string_table.size())
							throw std::runtime_error("PBF error: value string ID is out of bounds.");
						return string_table[i];
					}
				);

				node_callback(osm_node_id, latitude, longitude, tag_map);
			};

			auto decode_dense_node = [&](const char*begin, const char*end){
				const char
					*osm_node_id_begin = nullptr, *osm_node_id_end = nullptr,
					*key_value_pairs_begin = nullptr, *key_value_pairs_end = nullptr,
					*latitude_begin = nullptr, *latitude_end = nullptr,
					*longitude_begin = nullptr, *longitude_end = nullptr;

				decode_protobuf_message_with_callbacks(
					begin, end,
					[&](uint64_t key_id, uint64_t num){},
					[&](uint64_t key_id, double num){},
					[&](uint64_t key_id, const char*begin, const char*end){
						if(key_id == 1){
							osm_node_id_begin = begin;
							osm_node_id_end = end;
						}else if(key_id == 8){
							latitude_begin = begin;
							latitude_end = end;
						}else if(key_id == 9){
							longitude_begin = begin;
							longitude_end = end;
						}else if(key_id == 10){
							key_value_pairs_begin = begin;
							key_value_pairs_end = end;
						}
					}
				);

				if(osm_node_id_begin == nullptr)
					throw std::runtime_error("PBF error: dense node must contain node IDs.");
				if(latitude_begin == nullptr)
					throw std::runtime_error("PBF error: dense node must contain latitudes.");
				if(longitude_begin == nullptr)
					throw std::runtime_error("PBF error: dense node must contain longitudes.");

				tag_map.clear();

				uint64_t osm_node_id = 0;
				double latitude = 0.0, longitude = 0.0;
				while(osm_node_id_begin != osm_node_id_end){
					osm_node_id += zigzag_convert_uint64_to_int64(decode_varint_as_uint64_and_advance_first_parameter(osm_node_id_begin, osm_node_id_end));
					latitude += primblock_lon_offset + primblock_granularity * zigzag_convert_uint64_to_int64(decode_varint_as_uint64_and_advance_first_parameter(latitude_begin, latitude_end));
					longitude += primblock_lon_offset + primblock_granularity * zigzag_convert_uint64_to_int64(decode_varint_as_uint64_and_advance_first_parameter(longitude_begin, longitude_end));
					if(key_value_pairs_begin != nullptr){
						key_list.clear();
						value_list.clear();
						for(;;){
							uint64_t x = decode_varint_as_uint64_and_advance_first_parameter(key_value_pairs_begin, key_value_pairs_end);
							if(x == 0)
								break;
							uint64_t y = decode_varint_as_uint64_and_advance_first_parameter(key_value_pairs_begin, key_value_pairs_end);
							key_list.push_back(x);
							value_list.push_back(y);
						}
						tag_map.build(
							key_list.size(),
							[&](uint64_t i){
								i = key_list[i];
								if(i > string_table.size())
									throw std::runtime_error("PBF error: key string ID is out of bounds.");
								return string_table[i];
							},
							[&](uint64_t i){
								i = value_list[i];
								if(i > string_table.size())
									throw std::runtime_error("PBF error: value string ID is out of bounds.");
								return string_table[i];
							}
						);
					}
					node_callback(osm_node_id, latitude, longitude, tag_map);
				}
				if(latitude_begin != latitude_end)
					throw std::runtime_error("PBF error: dense node latitude array has a different length than the node ID array.");
				if(longitude_begin != longitude_end)
					throw std::runtime_error("PBF error: dense node longitude array has a different length than the node ID array.");
				if(key_value_pairs_begin != key_value_pairs_end)
					throw std::runtime_error("PBF error: dense node key-value array is too long.");


			};

			auto decode_way = [&](const char*begin, const char*end){
				uint64_t osm_way_id = (uint64_t)-1;

				const char
					*key_begin = nullptr, *key_end = nullptr,
					*value_begin = nullptr, *value_end = nullptr,
					*node_list_begin = nullptr, *node_list_end = nullptr;

				decode_protobuf_message_with_callbacks(
					begin, end,
					[&](uint64_t key_id, uint64_t num){
						if(key_id == 1){
							osm_way_id = num;
						}
					},
					[&](uint64_t key_id, double num){},
					[&](uint64_t key_id, const char*begin, const char*end){
						if(key_id == 2){
							key_begin = begin;
							key_end = end;
						}else if(key_id == 3){
							value_begin = begin;
							value_end = end;
						}else if(key_id == 8){
							node_list_begin = begin;
							node_list_end = end;
						}
					}
				);

				if(osm_way_id == (uint64_t)-1)
					throw std::runtime_error("PBF error: way is missing its OSM ID.");

				key_list.clear();
				value_list.clear();
				while(key_begin != key_end && value_begin != value_end){
					key_list.push_back(decode_varint_as_uint64_and_advance_first_parameter(key_begin, key_end));
					value_list.push_back(decode_varint_as_uint64_and_advance_first_parameter(value_begin, value_end));
				}
				if(key_begin != key_end || value_begin != value_end)
					throw std::runtime_error("PBF error: key and value arrays do not decode to equal length.");

				tag_map.build(
					key_list.size(),
					[&](uint64_t i){
						i = key_list[i];
						if(i > string_table.size())
							throw std::runtime_error("PBF error: key string ID is out of bounds.");
						return string_table[i];
					},
					[&](uint64_t i){
						i = value_list[i];
						if(i > string_table.size())
							throw std::runtime_error("PBF error: value string ID is out of bounds.");
						return string_table[i];
					}
				);

				node_list.clear();
				uint64_t id = 0;

				while(node_list_begin != node_list_end){
					id += zigzag_convert_uint64_to_int64(decode_varint_as_uint64_and_advance_first_parameter(node_list_begin, node_list_end));
					node_list.push_back(id);
				}

				way_callback(osm_way_id, node_list, tag_map);


			};

			auto decode_relation = [&](const char*begin, const char*end){
				uint64_t osm_relation_id = (uint64_t)-1;

				const char
					*key_begin = nullptr, *key_end = nullptr,
					*value_begin = nullptr, *value_end = nullptr,
					*member_role_begin = nullptr, *member_role_end = nullptr,
					*member_id_begin = nullptr, *member_id_end = nullptr,
					*member_type_begin = nullptr, *member_type_end = nullptr;


				decode_protobuf_message_with_callbacks(
					begin, end,
					[&](uint64_t key_id, uint64_t num){
						if(key_id == 1){
							osm_relation_id = num;
						}
					},
					[&](uint64_t key_id, double num){},
					[&](uint64_t key_id, const char*begin, const char*end){
						if(key_id == 2){
							key_begin = begin;
							key_end = end;
						}else if(key_id == 3){
							value_begin = begin;
							value_end = end;
						}else if(key_id == 8){
							member_role_begin = begin;
							member_role_end = end;
						}else if(key_id == 9){
							member_id_begin = begin;
							member_id_end = end;
						}else if(key_id == 10){
							member_type_begin = begin;
							member_type_end = end;
						}
					}
				);

				if(osm_relation_id == (uint64_t)-1)
					throw std::runtime_error("PBF error: way is missing its OSM ID.");

				key_list.clear();
				value_list.clear();
				while(key_begin != key_end && value_begin != value_end){
					key_list.push_back(decode_varint_as_uint64_and_advance_first_parameter(key_begin, key_end));
					value_list.push_back(decode_varint_as_uint64_and_advance_first_parameter(value_begin, value_end));
				}
				if(key_begin != key_end || value_begin != value_end)
					throw std::runtime_error("PBF error: key and value arrays do not decode to equal length.");

				tag_map.build(
					key_list.size(),
					[&](uint64_t i){
						i = key_list[i];
						if(i > string_table.size())
							throw std::runtime_error("PBF error: key string ID is out of bounds.");
						return string_table[i];
					},
					[&](uint64_t i){
						i = value_list[i];
						if(i > string_table.size())
							throw std::runtime_error("PBF error: value string ID is out of bounds.");
						return string_table[i];
					}
				);

				member_list.clear();

				uint64_t member_id = 0;
				while(member_id_begin != member_id_end){
					member_id += zigzag_convert_uint64_to_int64(decode_varint_as_uint64_and_advance_first_parameter(member_id_begin, member_id_end));

					auto x = decode_varint_as_uint64_and_advance_first_parameter(member_role_begin, member_role_end);
					if(x >= string_table.size())
						throw std::runtime_error("PBF error: relation member role string ID is out of bounds.");
					const char*role = string_table[x];
					uint64_t type_id = decode_varint_as_uint64_and_advance_first_parameter(member_type_begin, member_type_end);
					OSMIDType member_type;
					if(type_id == 0)
						member_type = OSMIDType::node;
					else if(type_id == 1)
						member_type = OSMIDType::way;
					else if(type_id == 2)
						member_type = OSMIDType::relation;
					else
						throw std::runtime_error("PBF error: Unknown relation type.");
					member_list.push_back({member_type, member_id, role});
				}
				relation_callback(osm_relation_id, member_list, tag_map);
			};

			for(auto g:group_list){
				decode_protobuf_message_with_callbacks(
					g.first, g.second,
					[&](uint64_t key_id, uint64_t num){},
					[&](uint64_t key_id, double num){},
					[&](uint64_t key_id, const char*begin, const char*end){
						if(key_id == 1 && node_callback) {
							decode_sparse_node(begin, end);
						} else if(key_id == 2 && node_callback) {
							decode_dense_node(begin, end);
						} else if(key_id == 3 && way_callback) {
							decode_way(begin, end);
						} else if(key_id == 4 && relation_callback) {
							decode_relation(begin, end);
						}
					}
				);
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
			std::atomic_thread_fence(std::memory_order::memory_order_seq_cst);
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

