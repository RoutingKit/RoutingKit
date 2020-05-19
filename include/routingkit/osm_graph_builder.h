#ifndef ROUTING_KIT_OSM_GRAPH_BUILDER_H
#define ROUTING_KIT_OSM_GRAPH_BUILDER_H

#include <routingkit/bit_vector.h>
#include <routingkit/tag_map.h>
#include <routingkit/osm_decoder.h>

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
	std::function<void(const std::string&)>log_message = nullptr,
	bool all_modelling_nodes_are_routing_nodes = false
);

enum class OSMWayDirectionCategory{
	open_in_both,
	only_open_forwards,
	only_open_backwards,
	closed
};

enum class OSMTurnRestrictionCategory{
	mandatory,
	prohibitive
};

enum class OSMTurnDirection{
	left_turn,
	right_turn,
	straight_on,
	u_turn

};

struct OSMTurnRestriction{
	uint64_t osm_relation_id;
	OSMTurnRestrictionCategory category;
	OSMTurnDirection direction;
	uint64_t from_way;
	uint64_t via_node;
	uint64_t to_way;
};

enum class OSMRoadGeometry{
	none,
	uncompressed,
	first_and_last
};

struct OSMRoutingGraph{
	std::vector<unsigned>first_out;
	std::vector<unsigned>head;
	std::vector<unsigned>way;
	std::vector<unsigned>geo_distance;
	std::vector<float>latitude;
	std::vector<float>longitude;
	std::vector<bool>is_arc_antiparallel_to_way;

	std::vector<unsigned>forbidden_turn_from_arc;
	std::vector<unsigned>forbidden_turn_to_arc;

	std::vector<unsigned>first_modelling_node;
	std::vector<float>modelling_node_latitude;
	std::vector<float>modelling_node_longitude;


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

	std::function<
		OSMWayDirectionCategory(
			uint64_t osm_way_id,
			unsigned routing_way_id,
			const TagMap&way_tags
		)
	>way_callback,

	std::function<
		void(
			uint64_t osm_relation_id,
			const std::vector<OSMRelationMember>&member_list,
			const TagMap&tags,
			std::function<void(OSMTurnRestriction)>
		)
	>turn_restriction_decoder,

	std::function<void(const std::string&)>log_message = nullptr,

	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false,

	OSMRoadGeometry geometry_to_be_extracted = OSMRoadGeometry::none
);

} // RoutingKit

#endif

