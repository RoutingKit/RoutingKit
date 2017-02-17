#ifndef OSM_DECODER_H
#define OSM_DECODER_H

#include <routingkit/tag_map.h>

#include <functional>
#include <stdint.h>
#include <string>
#include <vector>

namespace RoutingKit{

enum class OSMIDType{
	node,
	way,
	relation
};

struct OSMRelationMember{
	OSMIDType type;
	uint64_t id;
	const char*role;
};

void unordered_read_osm_pbf(
	const std::string&file_name,
	std::function<void(uint64_t osm_node_id, double latitude, double longitude, const TagMap&tags)>node_callback,
	std::function<void(uint64_t osm_way_id, const std::vector<uint64_t>&osm_node_id_list, const TagMap&tags)>way_callback,
	std::function<void(uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags)>relation_callback,
	std::function<void(const std::string&msg)>log_message = [](const std::string&){}
);

void ordered_read_osm_pbf(
	const std::string&file_name,
	std::function<void(uint64_t osm_node_id, double latitude, double longitude, const TagMap&tags)>node_callback,
	std::function<void(uint64_t osm_way_id, const std::vector<uint64_t>&osm_node_id_list, const TagMap&tags)>way_callback,
	std::function<void(uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags)>relation_callback,
	std::function<void(const std::string&msg)>log_message = [](const std::string&){},
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false
);

void speedtest_osm_pbf_reading(
	const std::string&pbf_file,
	std::function<void(std::string)>log_message
);

} // RoutingKit

#endif

