#include <routingkit/osm_graph_builder.h>
#include <routingkit/geo_dist.h>
#include <routingkit/timer.h>
#include <routingkit/sort.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/permutation.h>
#include <routingkit/graph_util.h>
#include <routingkit/bit_vector.h>
#include <routingkit/filter.h>
#include <routingkit/id_mapper.h>

#include "osm_pbf_decoder.h"

#include <vector>
#include <stdint.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <memory>

namespace RoutingKit{

OSMRoutingIDMapping load_osm_id_mapping_from_pbf(
	const std::string&file_name,
	std::function<bool(uint64_t, const TagMap&)>is_routing_node,
	std::function<bool(uint64_t, const TagMap&)>is_way_used_for_routing,
	std::function<void(std::string)>log_message
){
	OSMRoutingIDMapping map;

	log_message("Scanning OSM PBF data to determine IDs");
	long long timer = -get_micro_time();

	std::function<void(uint64_t,double,double,const TagMap&)>node_callback;
	if(is_routing_node){
		node_callback = [&](uint64_t osm_node_id, double lat, double lon, const TagMap&tags){
			if(!tags.empty()){
				if(is_routing_node(osm_node_id, tags)){
					map.is_modelling_node.make_large_enough_for(osm_node_id);
					map.is_modelling_node.set(osm_node_id);
					map.is_routing_node.make_large_enough_for(osm_node_id);
					map.is_routing_node.set(osm_node_id);
				}
			}
		};
	}


	unordered_read_osm_pbf(
		file_name,
		node_callback,
		[&](uint64_t osm_way_id, const std::vector<std::uint64_t>& osm_node_id_list, const TagMap&tags) {
			if(osm_node_id_list.size() >= 2 && is_way_used_for_routing(osm_way_id, tags)){
				map.is_routing_way.make_large_enough_for(osm_way_id);
				map.is_routing_way.set(osm_way_id);

				for(std::uint64_t osm_node_id : osm_node_id_list) {
					map.is_modelling_node.make_large_enough_for(osm_node_id);
					if(map.is_modelling_node.is_set(osm_node_id)) {
						map.is_routing_node.make_large_enough_for(osm_node_id);
						map.is_routing_node.set(osm_node_id);
					} else {
						map.is_modelling_node.set(osm_node_id);
					}
				}

				assert(map.is_modelling_node.is_set(osm_node_id_list.front()));
				assert(map.is_modelling_node.is_set(osm_node_id_list.back()));

				map.is_routing_node.make_large_enough_for(osm_node_id_list.front());
				map.is_routing_node.set(osm_node_id_list.front());
				map.is_routing_node.make_large_enough_for(osm_node_id_list.back());
				map.is_routing_node.set(osm_node_id_list.back());
			}
		},
		nullptr,
		log_message
	);
	timer += get_micro_time();
	log_message("Finished scan, needed "+std::to_string(timer)+"musec.");

	log_message("OSM ID range goes up to "+std::to_string(map.is_routing_node.size()) +" for routing nodes.");
	log_message("OSM ID range goes up to "+std::to_string(map.is_modelling_node.size()) +" for modelling nodes.");
	log_message("OSM ID range goes up to "+std::to_string(map.is_routing_way.size()) +" for routing ways.");
	log_message("Found "+std::to_string(map.is_routing_node.population_count()) +" routing nodes.");
	log_message("Found "+std::to_string(map.is_modelling_node.population_count()) +" modelling nodes.");
	log_message("Found "+std::to_string(map.is_routing_way.population_count()) +" routing ways.");

	return map;
}

OSMRoutingGraph load_osm_routing_graph_from_pbf(
	const std::string&pbf_file,

	const OSMRoutingIDMapping&mapping,
	std::function<OSMWayDirectionCategory(uint64_t, unsigned, const TagMap&)>way_callback,

	std::function<void(const std::string&)>log_message,
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered
){
	assert((mapping.is_modelling_node | mapping.is_routing_node) == mapping.is_modelling_node);

	std::vector<unsigned>tail;
	OSMRoutingGraph routing_graph;

	long long timer;

	log_message("Start computing ID mappings");
	timer = -get_micro_time();

	IDMapper modelling_node(mapping.is_modelling_node);
	IDMapper routing_node(mapping.is_routing_node);
	IDMapper routing_way(mapping.is_routing_way);

	timer += get_micro_time();
	log_message("Finished, needed "+std::to_string(timer)+"musec.");

	auto on_new_arc = [&](unsigned x, unsigned y, unsigned dist, unsigned routing_way_id, unsigned arc_path_head, unsigned arc_path_tail){
		tail.push_back(x);
		routing_graph.head.push_back(y);
		routing_graph.geo_distance.push_back(dist);
		routing_graph.way.push_back(routing_way_id);
		routing_graph.arc_path_head.push_back(arc_path_head);
		routing_graph.arc_path_tail.push_back(arc_path_tail);
	};

	std::vector<float>latitude(modelling_node.local_id_count());
	std::vector<float>longitude(modelling_node.local_id_count());

	log_message("Scanning OSM PBF data to load routing arcs");
	timer = -get_micro_time();
	ordered_read_osm_pbf(
		pbf_file,
		[&](uint64_t osm_node_id, double lat, double lon, const TagMap&tags){
			unsigned modelling_id = modelling_node.to_local(osm_node_id, invalid_id);
			if(modelling_id != invalid_id){
				latitude[modelling_id] = lat;
				longitude[modelling_id] = lon;
			}
		},
		[&](uint64_t osm_way_id, const std::vector<std::uint64_t> & node_list, const TagMap&tags) {
			unsigned routing_way_id = routing_way.to_local(osm_way_id, invalid_id);
			if(routing_way_id != invalid_id){
				OSMWayDirectionCategory dir = way_callback(osm_way_id, routing_way_id, tags);
				if(dir != OSMWayDirectionCategory::closed){
					unsigned modelling_id_of_previous_modelling_node = modelling_node.to_local(node_list[0]);
					unsigned routing_id_of_last_routing_node = routing_node.to_local(node_list[0]);

					double dist_since_last_routing_node = 0;

					/* TODO: implement optional polyline simplification, like Douglas-Peucker or Wang */

					std::vector<float> arc_path_latitude;
					std::vector<float> arc_path_longitude;

					arc_path_latitude.push_back(latitude[modelling_id_of_previous_modelling_node]);
					arc_path_longitude.push_back(longitude[modelling_id_of_previous_modelling_node]);

					for(unsigned i=1; i<node_list.size(); ++i){
						unsigned modelling_id_of_current_node = modelling_node.to_local(node_list[i]);

						arc_path_latitude.push_back(latitude[modelling_id_of_current_node]);
						arc_path_longitude.push_back(longitude[modelling_id_of_current_node]);

						dist_since_last_routing_node += geo_dist(
							latitude[modelling_id_of_current_node], longitude[modelling_id_of_current_node],
							latitude[modelling_id_of_previous_modelling_node], longitude[modelling_id_of_previous_modelling_node]
						);

						modelling_id_of_previous_modelling_node = modelling_id_of_current_node;

						unsigned routing_id_of_current_node = routing_node.to_local(node_list[i], invalid_id);
						if(routing_id_of_current_node != invalid_id){
							unsigned arc_path_head = routing_graph.arc_path_latitude.size();
							unsigned arc_path_tail = arc_path_head + arc_path_latitude.size();

							switch(dir){
							case OSMWayDirectionCategory::only_open_forwards:
								routing_graph.arc_path_latitude.insert(std::end(routing_graph.arc_path_latitude), std::begin(arc_path_latitude), std::end(arc_path_latitude));
								routing_graph.arc_path_longitude.insert(std::end(routing_graph.arc_path_longitude), std::begin(arc_path_longitude), std::end(arc_path_longitude));
								on_new_arc(routing_id_of_last_routing_node, routing_id_of_current_node, dist_since_last_routing_node, routing_way_id, arc_path_head, arc_path_tail);
								break;
							case OSMWayDirectionCategory::open_in_both:
								routing_graph.arc_path_latitude.insert(std::end(routing_graph.arc_path_latitude), std::begin(arc_path_latitude), std::end(arc_path_latitude));
								routing_graph.arc_path_longitude.insert(std::end(routing_graph.arc_path_longitude), std::begin(arc_path_longitude), std::end(arc_path_longitude));
								on_new_arc(routing_id_of_last_routing_node, routing_id_of_current_node, dist_since_last_routing_node, routing_way_id, arc_path_head, arc_path_tail);
								// no break
							case OSMWayDirectionCategory::only_open_backwards:
								/* the arc path is stored once; when displaying the arc backwards the client has to reorder,
								 * we already substract one so the client will get a valid head, but must decrease >= tail,
								 * why not arc_path_head -1? Because it can't handle 0;
								 */
								on_new_arc(routing_id_of_current_node, routing_id_of_last_routing_node, dist_since_last_routing_node, routing_way_id, arc_path_tail - 1, arc_path_head);
								break;
							default:
								assert(false);
							}

							arc_path_latitude.clear();
							arc_path_longitude.clear();
							dist_since_last_routing_node = 0;
							routing_id_of_last_routing_node = routing_id_of_current_node;

							arc_path_latitude.push_back(latitude[modelling_id_of_previous_modelling_node]);
							arc_path_longitude.push_back(longitude[modelling_id_of_previous_modelling_node]);
						}
					}
				}
			}
		},
		nullptr,
		log_message,
		file_is_ordered_even_though_file_header_says_that_it_is_unordered
	);
	timer += get_micro_time();
	log_message("Finished scan, needed "+std::to_string(timer)+"musec.");

	log_message("Found "+std::to_string(tail.size())+"arcs.");


	log_message("Start sorting arcs by tail");
	timer = -get_micro_time();

	{
		unsigned node_count = routing_node.local_id_count();

		auto p = compute_inverse_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(node_count, tail, routing_graph.head);
		routing_graph.head = apply_inverse_permutation(p, std::move(routing_graph.head));
		routing_graph.geo_distance = apply_inverse_permutation(p, std::move(routing_graph.geo_distance));
		routing_graph.way = apply_inverse_permutation(p, std::move(routing_graph.way));
		routing_graph.arc_path_head = apply_inverse_permutation(p, std::move(routing_graph.arc_path_head));
		routing_graph.arc_path_tail = apply_inverse_permutation(p, std::move(routing_graph.arc_path_tail));
		routing_graph.first_out = invert_vector(tail, node_count);
	}
	timer += get_micro_time();

	log_message("Finished sorting, needed "+std::to_string(timer)+"musec.");

	log_message("Start computing modelling to routing node mapping");
	timer = -get_micro_time();

	BitVector modelling_node_is_routing_node(modelling_node.local_id_count(), false);
	for(unsigned r=0; r < routing_node.local_id_count(); ++r)
		modelling_node_is_routing_node.set(modelling_node.to_local(routing_node.to_global(r)));

	timer += get_micro_time();
	log_message("Finished, needed "+std::to_string(timer)+"musec.");


	log_message("Start reducing geographic positions to routing nodes");
	timer = -get_micro_time();

	routing_graph.latitude = keep_element_of_vector_if(modelling_node_is_routing_node, latitude);
	routing_graph.longitude = keep_element_of_vector_if(modelling_node_is_routing_node, longitude);

	timer += get_micro_time();
	log_message("Finished, needed "+std::to_string(timer)+"musec.");

	return routing_graph; // NVRO
}

} // RoutingKit


