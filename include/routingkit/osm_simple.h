#ifndef ROUTING_KIT_OSM_SIMPLE_H
#define ROUTING_KIT_OSM_SIMPLE_H

#include <vector>
#include <functional>
#include <string>
#include <stdint.h>

namespace RoutingKit{


struct SimpleOSMCarRoutingGraph{
	std::vector<uint32_t>first_out;
	std::vector<uint32_t>head;
	std::vector<uint32_t>travel_time;
	std::vector<uint32_t>geo_distance;
	std::vector<uint32_t>arc_path_head;
	std::vector<uint32_t>arc_path_tail;
	std::vector<float>latitude;
	std::vector<float>longitude;
	std::vector<float>arc_path_latitude;
	std::vector<float>arc_path_longitude;

	unsigned node_count() const {
		return first_out.size()-1;
	}

	unsigned arc_count() const{
		return head.size();
	}

};

SimpleOSMCarRoutingGraph simple_load_osm_car_routing_graph_from_pbf(
	const std::string&pbf_file,
	const std::function<void(const std::string&)>&log_message = [](const std::string&){},
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false
);

struct SimpleOSMPedestrianRoutingGraph{
	std::vector<uint32_t>first_out;
	std::vector<uint32_t>head;
	std::vector<uint32_t>geo_distance;
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
	const std::function<void(const std::string&)>&log_message = [](const std::string&){},
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false
);

} // RoutingKit

#endif
