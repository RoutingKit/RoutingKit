#ifndef ROUTING_KIT_OSM_SIMPLE_H
#define ROUTING_KIT_OSM_SIMPLE_H

#include <vector>
#include <functional>
#include <string>

namespace RoutingKit{

struct SimpleOSMCarRoutingGraph{
	std::vector<unsigned>first_out;
	std::vector<unsigned>head;
	std::vector<unsigned>travel_time;
	std::vector<unsigned>geo_distance;
	std::vector<float>latitude;
	std::vector<float>longitude;
	std::vector<unsigned>forbidden_turn_from_arc;
	std::vector<unsigned>forbidden_turn_to_arc;

	unsigned node_count() const {
		return first_out.size()-1;
	}

	unsigned arc_count() const{
		return head.size();
	}

};

SimpleOSMCarRoutingGraph simple_load_osm_car_routing_graph_from_pbf(
	const std::string&pbf_file,
	const std::function<void(const std::string&)>&log_message = nullptr,
	bool all_modelling_nodes_are_routing_nodes = false,
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false
);

struct SimpleOSMPedestrianRoutingGraph{
	std::vector<unsigned>first_out;
	std::vector<unsigned>head;
	std::vector<unsigned>geo_distance;
	std::vector<float>latitude;
	std::vector<float>longitude;

	unsigned node_count() const {
		return first_out.size()-1;
	}

	unsigned arc_count() const{
		return head.size();
	}

};

SimpleOSMPedestrianRoutingGraph simple_load_osm_pedestrian_routing_graph_from_pbf(
	const std::string&pbf_file,
	const std::function<void(const std::string&)>&log_message = nullptr,
	bool all_modelling_nodes_are_routing_nodes = false,
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false
);

struct SimpleOSMBicycleRoutingGraph{
	std::vector<unsigned>first_out;
	std::vector<unsigned>head;
	std::vector<unsigned>geo_distance;
	std::vector<float>latitude;
	std::vector<float>longitude;
	std::vector<unsigned char>arc_comfort_level;

	unsigned node_count() const {
		return first_out.size()-1;
	}

	unsigned arc_count() const{
		return head.size();
	}

};

SimpleOSMBicycleRoutingGraph simple_load_osm_bicycle_routing_graph_from_pbf(
	const std::string&pbf_file,
	const std::function<void(const std::string&)>&log_message = nullptr,
	bool all_modelling_nodes_are_routing_nodes = false,
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false
);

// These functions are declared in osm_profile.h
// We redeclare them here to avoid having to include osm_profile.h in every 
// file that uses the bicycle routing graph.
unsigned char get_min_bicycle_comfort_level();
unsigned char get_max_bicycle_comfort_level();

} // RoutingKit

#endif
