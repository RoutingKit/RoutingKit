#include <routingkit/osm_graph_builder.h>
#include <routingkit/osm_profile.h>
#include <routingkit/osm_simple.h>

#include <vector>
#include <stdint.h>
#include <string>

namespace RoutingKit{

SimpleOSMCarRoutingGraph simple_load_osm_car_routing_graph_from_pbf(
	const std::string&pbf_file,
	const std::function<void(const std::string&)>&log_message,
	bool all_modelling_nodes_are_routing_nodes,
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered
){
	auto mapping = load_osm_id_mapping_from_pbf(
		pbf_file,
		nullptr,
		[&](uint64_t osm_way_id, const TagMap&tags){
			return is_osm_way_used_by_cars(osm_way_id, tags, log_message);
		},
		log_message,
		all_modelling_nodes_are_routing_nodes
	);

	unsigned routing_way_count = mapping.is_routing_way.population_count();
	std::vector<unsigned>way_speed(routing_way_count);

	auto routing_graph = load_osm_routing_graph_from_pbf(
		pbf_file,
		mapping,
		[&](uint64_t osm_way_id, unsigned routing_way_id, const TagMap&way_tags){
			way_speed[routing_way_id] = get_osm_way_speed(osm_way_id, way_tags, log_message);
			return get_osm_car_direction_category(osm_way_id, way_tags, log_message);
		},
		[&](uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags, std::function<void(OSMTurnRestriction)>on_new_restriction){
			return decode_osm_car_turn_restrictions(osm_relation_id, member_list, tags, on_new_restriction, log_message);
		},
		log_message
	);

	mapping = OSMRoutingIDMapping(); // release memory

	SimpleOSMCarRoutingGraph ret;
	ret.first_out = std::move(routing_graph.first_out);
	ret.head = std::move(routing_graph.head);
	ret.geo_distance = std::move(routing_graph.geo_distance);
	ret.latitude = std::move(routing_graph.latitude);
	ret.longitude = std::move(routing_graph.longitude);

	ret.travel_time = ret.geo_distance;
	for(unsigned a=0; a<ret.travel_time.size(); ++a){
		ret.travel_time[a] *= 18000;
		ret.travel_time[a] /= way_speed[routing_graph.way[a]];
		ret.travel_time[a] /= 5;
	}

	ret.forbidden_turn_from_arc = std::move(routing_graph.forbidden_turn_from_arc);
	assert(is_sorted_using_less(ret.forbidden_turn_from_arc));
	ret.forbidden_turn_to_arc = std::move(routing_graph.forbidden_turn_to_arc);

	return ret;
}

SimpleOSMPedestrianRoutingGraph simple_load_osm_pedestrian_routing_graph_from_pbf(
	const std::string&pbf_file,
	const std::function<void(const std::string&)>&log_message,
	bool all_modelling_nodes_are_routing_nodes,
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered
){
	auto mapping = load_osm_id_mapping_from_pbf(
		pbf_file,
		nullptr,
		[&](uint64_t osm_way_id, const TagMap&tags){
			return is_osm_way_used_by_pedestrians(osm_way_id, tags, log_message);
		},
		log_message,
		all_modelling_nodes_are_routing_nodes
	);

	auto routing_graph = load_osm_routing_graph_from_pbf(
		pbf_file,
		mapping,
		[&](uint64_t osm_way_id, unsigned routing_way_id, const TagMap&way_tags){
			return OSMWayDirectionCategory::open_in_both;
		},
		nullptr,
		log_message
	);

	mapping = OSMRoutingIDMapping(); // release memory

	SimpleOSMPedestrianRoutingGraph ret;
	ret.first_out = std::move(routing_graph.first_out);
	ret.head = std::move(routing_graph.head);
	ret.geo_distance = std::move(routing_graph.geo_distance);
	ret.latitude = std::move(routing_graph.latitude);
	ret.longitude = std::move(routing_graph.longitude);

	return ret;
}


SimpleOSMBicycleRoutingGraph simple_load_osm_bicycle_routing_graph_from_pbf(
	const std::string&pbf_file,
	const std::function<void(const std::string&)>&log_message,
	bool all_modelling_nodes_are_routing_nodes,
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered
){
	auto mapping = load_osm_id_mapping_from_pbf(
		pbf_file,
		nullptr,
		[&](uint64_t osm_way_id, const TagMap&tags){
			return is_osm_way_used_by_bicycles(osm_way_id, tags, log_message);
		},
		log_message,
		all_modelling_nodes_are_routing_nodes
	);

	unsigned routing_way_count = mapping.is_routing_way.population_count();

	std::vector<unsigned char> comfort_level(routing_way_count, false);

	auto routing_graph = load_osm_routing_graph_from_pbf(
		pbf_file,
		mapping,
		[&](uint64_t osm_way_id, unsigned routing_way_id, const TagMap&way_tags){
			comfort_level[routing_way_id] = get_osm_way_bicycle_comfort_level(osm_way_id, way_tags, log_message);
			return get_osm_bicycle_direction_category(osm_way_id, way_tags, log_message);
		},
		nullptr,
		log_message
	);

	unsigned arc_count = routing_graph.head.size();

	mapping = OSMRoutingIDMapping(); // release memory

	SimpleOSMBicycleRoutingGraph ret;
	ret.first_out = std::move(routing_graph.first_out);
	ret.head = std::move(routing_graph.head);
	ret.geo_distance = std::move(routing_graph.geo_distance);
	ret.latitude = std::move(routing_graph.latitude);
	ret.longitude = std::move(routing_graph.longitude);

	ret.arc_comfort_level.resize(arc_count);
	for(unsigned a=0; a<arc_count; ++a)
		ret.arc_comfort_level[a] = comfort_level[routing_graph.way[a]];

	return ret;
}

} // RoutingKit

