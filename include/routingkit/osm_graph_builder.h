#ifndef ROUTING_KIT_OSM_GRAPH_BUILDER_H
#define ROUTING_KIT_OSM_GRAPH_BUILDER_H

#include <routingkit/bit_vector.h>
#include <routingkit/tag_map.h>

#include <vector>
#include <functional>
#include <string>
#include <stdint.h>

namespace RoutingKit{

struct OSMRoutingIDMapping{
	BitVector is_modelling_node;
	BitVector is_routing_node;
	BitVector is_routing_way;
};

OSMRoutingIDMapping load_osm_id_mapping_from_pbf(
	const std::string&file_name,
	std::function<bool(uint64_t osm_node_id, const TagMap&node_tags)>is_routing_node, // returns true if node should be a routing node
	std::function<bool(uint64_t osm_way_id, const TagMap&way_tags)>is_way_used_for_routing, // return true if way should be a routing way
	std::function<void(std::string)>log_message = [](const std::string&){}
);

enum class OSMWayDirectionCategory{
	open_in_both,
	only_open_forwards,
	only_open_backwards,
	closed
};

struct OSMRoutingGraph{
	std::vector<uint32_t>first_out;
	std::vector<uint32_t>head;
	std::vector<uint32_t>way;
	std::vector<uint32_t>geo_distance;
	std::vector<uint32_t>arc_path_head;
	std::vector<uint32_t>arc_path_tail;
	std::vector<float>latitude;
	std::vector<float>longitude;
	std::vector<float>arc_path_latitude;
	std::vector<float>arc_path_longitude;

	unsigned node_count()const{
		return first_out.size()-1;
	}

	unsigned arc_count()const{
		return head.size();
	}
};

OSMRoutingGraph load_osm_routing_graph_from_pbf(
	const std::string&pbf_file,

	const OSMRoutingIDMapping&mapping,
	std::function<OSMWayDirectionCategory(uint64_t osm_way_id, unsigned routing_way_id, const TagMap&way_tags)>way_callback, // return in which direction a way is open

	std::function<void(const std::string&)>log_message = [](const std::string&){},
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false
);

} // RoutingKit

#endif

