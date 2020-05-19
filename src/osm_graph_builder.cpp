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
#include <routingkit/osm_decoder.h>

#include <vector>
#include <stdint.h>
#include <string>
#include <stdio.h>
#include <memory>

namespace RoutingKit{

OSMRoutingIDMapping load_osm_id_mapping_from_pbf(
	const std::string&file_name,
	std::function<bool(uint64_t, const TagMap&)>is_routing_node,
	std::function<bool(uint64_t, const TagMap&)>is_way_used_for_routing,
	std::function<void(const std::string&)>log_message,
	bool all_modelling_nodes_are_routing_nodes
){
	OSMRoutingIDMapping map;

	long long timer=0;

	if(log_message){
		log_message("Scanning OSM PBF data to determine IDs");
		if(all_modelling_nodes_are_routing_nodes)
			log_message("All modelling nodes are routing nodes");
		else
			log_message("Not all modelling nodes are routing nodes");
		timer = -get_micro_time();
	}

	std::function<void(uint64_t,double,double,const TagMap&)>node_callback;
	if(is_routing_node){
		node_callback = [&](uint64_t osm_node_id, double lat, double lon, const TagMap&tags){
			if(is_routing_node(osm_node_id, tags)){
				map.is_modelling_node.make_large_enough_for(osm_node_id);
				map.is_modelling_node.set(osm_node_id);
				map.is_routing_node.make_large_enough_for(osm_node_id);
				map.is_routing_node.set(osm_node_id);
			}
		};
	}

	std::function<void(uint64_t,const std::vector<std::uint64_t>& osm_node_id_list,const TagMap&)>way_callback;
	if(!all_modelling_nodes_are_routing_nodes){
		way_callback = [&](uint64_t osm_way_id, const std::vector<std::uint64_t>& osm_node_id_list, const TagMap&tags) {
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
		};
	}else{
		way_callback = [&](uint64_t osm_way_id, const std::vector<std::uint64_t>& osm_node_id_list, const TagMap&tags) {
			if(osm_node_id_list.size() >= 2 && is_way_used_for_routing(osm_way_id, tags)){
				map.is_routing_way.make_large_enough_for(osm_way_id);
				map.is_routing_way.set(osm_way_id);

				for(std::uint64_t osm_node_id : osm_node_id_list) {
					map.is_modelling_node.make_large_enough_for(osm_node_id);
					map.is_routing_node.make_large_enough_for(osm_node_id);

					map.is_routing_node.set(osm_node_id);
					map.is_modelling_node.set(osm_node_id);
				}

				assert(map.is_modelling_node.is_set(osm_node_id_list.front()));
				assert(map.is_modelling_node.is_set(osm_node_id_list.back()));
			}
		};
	}

	unordered_read_osm_pbf(file_name, node_callback, way_callback, nullptr, log_message);

	if(log_message){
		timer += get_micro_time();
		log_message("Finished scan, needed "+std::to_string(timer)+" musec.");

		log_message("OSM ID range goes up to "+std::to_string(map.is_routing_node.size()) +" for routing nodes.");
		log_message("OSM ID range goes up to "+std::to_string(map.is_modelling_node.size()) +" for modelling nodes.");
		log_message("OSM ID range goes up to "+std::to_string(map.is_routing_way.size()) +" for routing ways.");
		log_message("Found "+std::to_string(map.is_routing_node.population_count()) +" routing nodes.");
		log_message("Found "+std::to_string(map.is_modelling_node.population_count()) +" modelling nodes.");
		log_message("Found "+std::to_string(map.is_routing_way.population_count()) +" routing ways.");
	}

	return map;
}

OSMRoutingGraph load_osm_routing_graph_from_pbf(
	const std::string&pbf_file,
	const OSMRoutingIDMapping&mapping,
	std::function<OSMWayDirectionCategory(uint64_t, unsigned, const TagMap&)>way_callback,
	std::function<
		void(
			uint64_t osm_relation_id,
			const std::vector<OSMRelationMember>&member_list,
			const TagMap&tags,
			std::function<void(OSMTurnRestriction)>
		)
	>turn_restriction_decoder,
	std::function<void(const std::string&)>log_message,
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered,
	OSMRoadGeometry geometry_to_be_extracted
){
	assert((mapping.is_modelling_node | mapping.is_routing_node) == mapping.is_modelling_node);

	if(!way_callback){
		way_callback = [](uint64_t, unsigned, const TagMap&){ return OSMWayDirectionCategory::open_in_both; };
	}

	if(turn_restriction_decoder && geometry_to_be_extracted == OSMRoadGeometry::none){
		geometry_to_be_extracted = OSMRoadGeometry::first_and_last;
	}

	std::vector<unsigned>tail;
	OSMRoutingGraph routing_graph;

	long long timer=0;

	if(log_message){
		log_message("Start computing ID mappings");
		timer = -get_micro_time();
	}

	IDMapper modelling_node(mapping.is_modelling_node);
	IDMapper routing_node(mapping.is_routing_node);
	IDMapper routing_way(mapping.is_routing_way);

	if(log_message){
		timer += get_micro_time();
		log_message("Finished, needed "+std::to_string(timer)+" musec.");
	}

	auto on_new_arc = [&](
		unsigned x, unsigned y, unsigned dist, unsigned routing_way_id, bool is_antiparallel_to_way,
		const std::vector<float>&modelling_node_latitude,
		const std::vector<float>&modelling_node_longitude)
	{
		tail.push_back(x);
		routing_graph.head.push_back(y);
		routing_graph.geo_distance.push_back(dist);
		routing_graph.way.push_back(routing_way_id);
		routing_graph.is_arc_antiparallel_to_way.push_back(is_antiparallel_to_way);
		if(geometry_to_be_extracted == OSMRoadGeometry::uncompressed){
			routing_graph.first_modelling_node.push_back(routing_graph.modelling_node_latitude.size());
			routing_graph.modelling_node_latitude.insert(
				routing_graph.modelling_node_latitude.end(),
				modelling_node_latitude.begin(), modelling_node_latitude.end()
			);
			routing_graph.modelling_node_longitude.insert(
				routing_graph.modelling_node_longitude.end(),
				modelling_node_longitude.begin(), modelling_node_longitude.end()
			);
		}else if(geometry_to_be_extracted == OSMRoadGeometry::first_and_last){
			routing_graph.first_modelling_node.push_back(routing_graph.modelling_node_latitude.size());
			if(modelling_node_latitude.size() == 1){
				routing_graph.modelling_node_latitude.push_back(modelling_node_latitude.front());
				routing_graph.modelling_node_longitude.push_back(modelling_node_longitude.front());
			}else if(!modelling_node_latitude.empty()){
				routing_graph.modelling_node_latitude.push_back(modelling_node_latitude.front());
				routing_graph.modelling_node_longitude.push_back(modelling_node_longitude.front());
				routing_graph.modelling_node_latitude.push_back(modelling_node_latitude.back());
				routing_graph.modelling_node_longitude.push_back(modelling_node_longitude.back());
			}
		}
	};

	std::vector<OSMTurnRestriction>osm_turn_restrictions;
	std::function<void(uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags)>relation_callback = nullptr;

	if(turn_restriction_decoder){
		relation_callback = [&](
			uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags
		){
			turn_restriction_decoder(
				osm_relation_id, member_list, tags,
				[&](OSMTurnRestriction restriction){
					osm_turn_restrictions.push_back(restriction);
				}
			);
		};
	}

	std::vector<float>latitude(modelling_node.local_id_count());
	std::vector<float>longitude(modelling_node.local_id_count());
	std::vector<float>modelling_node_latitude;
	std::vector<float>modelling_node_longitude;

	if(log_message){
		log_message("Scanning OSM PBF data to load routing arcs");
		timer = -get_micro_time();
	}
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

					for(unsigned i=1; i<node_list.size(); ++i){
						unsigned modelling_id_of_current_node = modelling_node.to_local(node_list[i]);

						dist_since_last_routing_node += geo_dist(
							latitude[modelling_id_of_current_node], longitude[modelling_id_of_current_node],
							latitude[modelling_id_of_previous_modelling_node], longitude[modelling_id_of_previous_modelling_node]
						);
						if(geometry_to_be_extracted == OSMRoadGeometry::uncompressed || geometry_to_be_extracted == OSMRoadGeometry::first_and_last){
							modelling_node_latitude.push_back(latitude[modelling_id_of_current_node]);
							modelling_node_longitude.push_back(longitude[modelling_id_of_current_node]);
						}

						modelling_id_of_previous_modelling_node = modelling_id_of_current_node;

						unsigned routing_id_of_current_node = routing_node.to_local(node_list[i], invalid_id);
						if(routing_id_of_current_node != invalid_id){

							if(geometry_to_be_extracted == OSMRoadGeometry::uncompressed || geometry_to_be_extracted == OSMRoadGeometry::first_and_last){
								modelling_node_latitude.pop_back();
								modelling_node_longitude.pop_back();
							}

							switch(dir){
							case OSMWayDirectionCategory::only_open_forwards:
								on_new_arc(routing_id_of_last_routing_node, routing_id_of_current_node, dist_since_last_routing_node, routing_way_id, false, modelling_node_latitude, modelling_node_longitude);
								break;
							case OSMWayDirectionCategory::open_in_both:
								on_new_arc(routing_id_of_last_routing_node, routing_id_of_current_node, dist_since_last_routing_node, routing_way_id, false, modelling_node_latitude, modelling_node_longitude);
								// no break
							case OSMWayDirectionCategory::only_open_backwards:
								std::reverse(modelling_node_latitude.begin(), modelling_node_latitude.end());
								std::reverse(modelling_node_longitude.begin(), modelling_node_longitude.end());
								on_new_arc(routing_id_of_current_node, routing_id_of_last_routing_node, dist_since_last_routing_node, routing_way_id, true, modelling_node_latitude, modelling_node_longitude);
								break;
							default:
								assert(false);
							}

							dist_since_last_routing_node = 0;
							modelling_node_latitude.clear();
							modelling_node_longitude.clear();
							routing_id_of_last_routing_node = routing_id_of_current_node;
						}
					}
				}
			}
		},
		relation_callback,
		log_message,
		file_is_ordered_even_though_file_header_says_that_it_is_unordered
	);

	if(log_message){
		timer += get_micro_time();
		log_message("Finished scan, needed "+std::to_string(timer)+" musec.");
		log_message("Found "+std::to_string(tail.size())+" arcs.");
		log_message("Start sorting arcs by tail");
		timer = -get_micro_time();
	}

	{
		unsigned node_count = routing_node.local_id_count();

		auto p = compute_inverse_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(node_count, tail, routing_graph.head);
		routing_graph.head = apply_inverse_permutation(p, std::move(routing_graph.head));
		routing_graph.geo_distance = apply_inverse_permutation(p, std::move(routing_graph.geo_distance));
		routing_graph.way = apply_inverse_permutation(p, std::move(routing_graph.way));
		routing_graph.is_arc_antiparallel_to_way = apply_inverse_permutation(p, std::move(routing_graph.is_arc_antiparallel_to_way));
		routing_graph.first_out = invert_vector(tail, node_count);

		if(geometry_to_be_extracted == OSMRoadGeometry::uncompressed || geometry_to_be_extracted == OSMRoadGeometry::first_and_last){
			routing_graph.first_modelling_node.push_back(routing_graph.modelling_node_latitude.size());

			std::vector<unsigned>first_modelling_node;
			std::vector<float>modelling_node_latitude;
			std::vector<float>modelling_node_longitude;

			first_modelling_node.reserve(routing_graph.first_modelling_node.size());
			modelling_node_latitude.reserve(routing_graph.modelling_node_latitude.size());
			modelling_node_longitude.reserve(routing_graph.modelling_node_longitude.size());

			auto new_arc_id_to_old_arc_id = invert_permutation(p);
			for(auto old_arc_id : new_arc_id_to_old_arc_id){
				first_modelling_node.push_back(modelling_node_latitude.size());

				int first = routing_graph.first_modelling_node[old_arc_id];
				int last = routing_graph.first_modelling_node[old_arc_id + 1];

				modelling_node_latitude.insert(
					modelling_node_latitude.end(),
					routing_graph.modelling_node_latitude.begin() + first,
					routing_graph.modelling_node_latitude.begin() + last
				);
				modelling_node_longitude.insert(
					modelling_node_longitude.end(),
					routing_graph.modelling_node_longitude.begin() + first,
					routing_graph.modelling_node_longitude.begin() + last
				);
			}

			first_modelling_node.push_back(modelling_node_latitude.size());

			routing_graph.first_modelling_node = std::move(first_modelling_node);
			routing_graph.modelling_node_latitude = std::move(modelling_node_latitude);
			routing_graph.modelling_node_longitude = std::move(modelling_node_longitude);
		}
	}

	if(log_message){
		timer += get_micro_time();
		log_message("Finished sorting, needed "+std::to_string(timer)+" musec.");
		log_message("Start computing modelling to routing node mapping");
		timer = -get_micro_time();
	}

	BitVector modelling_node_is_routing_node(modelling_node.local_id_count(), false);
	for(unsigned r=0; r < routing_node.local_id_count(); ++r)
		modelling_node_is_routing_node.set(modelling_node.to_local(routing_node.to_global(r)));

	if(log_message){
		timer += get_micro_time();
		log_message("Finished, needed "+std::to_string(timer)+" musec.");

		log_message("Start reducing geographic positions to routing nodes");
		timer = -get_micro_time();
	}

	routing_graph.latitude = keep_element_of_vector_if(modelling_node_is_routing_node, latitude);
	routing_graph.longitude = keep_element_of_vector_if(modelling_node_is_routing_node, longitude);

	if(log_message){
		timer += get_micro_time();
		log_message("Finished, needed "+std::to_string(timer)+" musec.");
		log_message("Found "+std::to_string(osm_turn_restrictions.size())+" OSM turn restrictions.");
	}

	if(!osm_turn_restrictions.empty()){
		auto&first_out = routing_graph.first_out;
		auto&head = routing_graph.head;
		auto&way = routing_graph.way;

		const unsigned node_count = first_out.size()-1;
		const unsigned arc_count = head.size();
		const unsigned way_count = routing_way.local_id_count();

		{
			if(log_message){
				log_message("Start mapping IDs in turn restrictions");
				timer = -get_micro_time();
			}

			auto
				in = osm_turn_restrictions.begin(),
				out = osm_turn_restrictions.begin(),
				end = osm_turn_restrictions.end();

			while(in != end){
				bool has_error = false;

				unsigned local_from_way = routing_way.to_local(in->from_way, invalid_id);
				if(local_from_way == invalid_id){
					has_error = true;
				}

				unsigned local_to_way = routing_way.to_local(in->to_way, invalid_id);
				if(local_to_way == invalid_id){
					has_error = true;
				}

				unsigned local_via_node = invalid_id;
				if(in->via_node != (std::uint64_t)-1){
					local_via_node = routing_node.to_local(in->via_node, invalid_id);
					if(local_via_node == invalid_id){
						has_error = true;
					}
				}

				if(has_error){
					++in;
				} else {
					out->osm_relation_id = in->osm_relation_id;
					out->category = in->category;
					out->direction = in->direction;
					out->from_way = local_from_way;
					out->to_way = local_to_way;
					out->via_node = local_via_node;
					++in;
					++out;
				}
			}

			osm_turn_restrictions.erase(out, end);

			if(log_message){
				timer += get_micro_time();
				log_message("Finished, needed "+std::to_string(timer)+" musec.");
			}
		}

		if(log_message){
			log_message("After removing restrictions with not exported ways or node only "+std::to_string(osm_turn_restrictions.size())+" restrictions remain.");
		}

		std::vector<unsigned>forbidden_from;
		std::vector<unsigned>forbidden_to;

		auto add_forbidden_turn = [&](unsigned from_arc, unsigned to_arc){
			assert(from_arc < arc_count);
			assert(to_arc < arc_count);

			forbidden_from.push_back(from_arc);
			forbidden_to.push_back(to_arc);
		};

		{

			if(log_message){
				log_message("Sorting arcs by way");
				timer = -get_micro_time();
			}

			auto index_to_arc = compute_sort_permutation_using_key(way, way_count, [](unsigned x){return x;});
			auto first_index_of_way = invert_vector(apply_permutation(index_to_arc, way), way_count);

			if(log_message){
				timer += get_micro_time();
				log_message("Finished, needed "+std::to_string(timer)+" musec.");
			}

			{
				if(log_message){
					log_message("Start filling in missing via node IDs in turn restrictions and handling mandatory turns with from = to");
					timer = -get_micro_time();
				}

				std::vector<bool> is_head_of_from_way(node_count, false);
				std::vector<unsigned> incomming_arc_of_way_into_node(node_count, invalid_id);

				unsigned repaired_count = 0;
				unsigned no_via_count = 0;
				unsigned multiple_via_count = 0;
				unsigned go_straight_count = 0;

				auto
					in = osm_turn_restrictions.begin(),
					out = osm_turn_restrictions.begin(),
					end = osm_turn_restrictions.end();

				while(in != end){
					if(in->via_node != invalid_id) {
						*out = *in;
						++out;
						++in;
					} else if(in->from_way == in->to_way && in->category == OSMTurnRestrictionCategory::mandatory){
						for(auto i=first_index_of_way[in->from_way]; i!=first_index_of_way[in->from_way+1]; ++i){
							auto arc = index_to_arc[i];
							incomming_arc_of_way_into_node[head[arc]] = arc;
						}

						for(auto i=first_index_of_way[in->from_way]; i!=first_index_of_way[in->from_way+1]; ++i){
							auto out_arc = index_to_arc[i];
							auto via_node = tail[out_arc];
							auto in_arc = incomming_arc_of_way_into_node[via_node];
							if(in_arc != invalid_id)
								for(unsigned not_out_arc=first_out[via_node]; not_out_arc!=first_out[via_node+1]; ++not_out_arc)
									if(not_out_arc != out_arc)
										add_forbidden_turn(in_arc, not_out_arc);
						}

						for(auto i=first_index_of_way[in->from_way]; i!=first_index_of_way[in->from_way+1]; ++i){
							auto arc = index_to_arc[i];
							incomming_arc_of_way_into_node[head[arc]] = invalid_id;
						}
						++in;
						++go_straight_count;
					} else {
						unsigned via_node = invalid_id;
						unsigned second_via_node = invalid_id;

						for(auto i=first_index_of_way[in->from_way]; i!=first_index_of_way[in->from_way+1]; ++i){
							auto arc = index_to_arc[i];
							is_head_of_from_way[head[arc]] = true;
						}

						for(auto i=first_index_of_way[in->to_way]; i!=first_index_of_way[in->to_way+1]; ++i){
							auto arc = index_to_arc[i];
							if(is_head_of_from_way[tail[arc]]){
								if(via_node != invalid_id){
									second_via_node = tail[arc];
								} else {
									via_node = tail[arc];
								}
							}
						}

						for(auto i=first_index_of_way[in->from_way]; i!=first_index_of_way[in->from_way+1]; ++i){
							auto arc = index_to_arc[i];
							is_head_of_from_way[head[arc]] = false;
						}

						if(via_node == invalid_id){
							if(log_message)
								log_message(
									"Turn restriction with OSM-relation-ID \""+std::to_string(in->osm_relation_id)+"\" "
									"with OSM-way-from-ID \""+std::to_string(routing_way.to_global(in->from_way))+"\" "
									"and OSM-way to-ID \""+std::to_string(routing_way.to_global(in->to_way))+"\" "
									"does not have a via-node "
									"and their ways do not cross, ingoring restriction"
								);
							++no_via_count;
							++in;
						}else if(second_via_node != invalid_id){
							if(log_message)
								log_message(
									"Turn restriction with OSM-relation-ID \""+std::to_string(in->osm_relation_id)+"\" "
									"with OSM-way-from-ID \""+std::to_string(routing_way.to_global(in->from_way))+"\" "
									"and OSM-way-to-ID \""+std::to_string(routing_way.to_global(in->to_way))+"\" "
									"does not have a via-node "
									"and there are multiple ambiguous candidates, namely OSM-node-IDs \""+std::to_string(routing_node.to_global(via_node))+"\" "
									"and \""+std::to_string(routing_node.to_global(second_via_node))+"\" (and maybe more), ingoring restriction"
								);
							++multiple_via_count;
							++in;
						}else{
							++repaired_count;

							out->osm_relation_id = in->osm_relation_id;
							out->direction = in->direction;
							out->category = in->category;
							out->from_way = in->from_way;
							out->to_way = in->to_way;
							out->via_node = via_node;

							++out;
							++in;
						}
					}
				}

				if(log_message){
					log_message("There were "+std::to_string(go_straight_count)+" go-straight turns that were expanded into "+std::to_string(forbidden_from.size())+" forbidden turns.");
					log_message("There were "+std::to_string(repaired_count)+" normal turns without via-node for which a via-node could be derived.");
					log_message("There were "+std::to_string(no_via_count)+" normal turns without via-node discarded because no potential via-node was found.");
					log_message("There were "+std::to_string(no_via_count)+" normal turns without via-node discarded because multiple potential via-node were found.");
				}

				osm_turn_restrictions.erase(out, end);

				if(log_message){
					timer += get_micro_time();
					log_message("Finished, needed "+std::to_string(timer)+" musec.");
				}
			}
		}

		{
			if(log_message){
				log_message("Building forbidden turns");
				timer = -get_micro_time();
			}

			const unsigned node_count = routing_graph.first_out.size()-1;
			const unsigned arc_count = routing_graph.head.size();
			(void)arc_count;

			auto in_arc = compute_sort_permutation_using_key(routing_graph.head, node_count, [](unsigned x){return x;});
			auto first_in = invert_vector(apply_permutation(in_arc, routing_graph.head), node_count);

			for(auto x:osm_turn_restrictions){
				assert(x.via_node != invalid_id);
				assert(x.from_way != invalid_id);
				assert(x.to_way != invalid_id);

				std::vector<unsigned>from_candidates;
				for(unsigned i=first_in[x.via_node]; i!=first_in[x.via_node+1]; ++i){
					assert(i < arc_count);
					unsigned arc = in_arc[i];
					assert(arc < arc_count);
					if(routing_graph.way[arc] == x.from_way){
						from_candidates.push_back(arc);
					}
				}
				if(from_candidates.empty()){
					// if(log_message){
					// 	log_message(
					// 		"Cannot find from-arc for turn restriction with OSM relation ID \""+std::to_string(x.osm_relation_id)+"\" "
					// 		"and OSM from-way \""+std::to_string(routing_way.to_global(x.from_way))+"\" "
					// 		"and OSM to-way \""+std::to_string(routing_way.to_global(x.to_way))+"\" "
					// 		"and OSM via-node \""+std::to_string(routing_node.to_global(x.via_node))+"\""
					// 		", ignoring restriction"
					// 	);
					// }
					continue;
				}
				#ifndef NDEBUG
				for(auto from:from_candidates)
					assert(head[from] == x.via_node);
				#endif

				std::vector<unsigned>to_candidates;
				for(unsigned arc=routing_graph.first_out[x.via_node]; arc!=routing_graph.first_out[x.via_node+1]; ++arc){
					assert(arc < arc_count);
					if(routing_graph.way[arc] == x.to_way){
						to_candidates.push_back(arc);
					}
				}
				if(to_candidates.empty()){
					// if(log_message){
					// 	log_message(
					// 		"Cannot find to-arc for turn restriction with OSM relation ID \""+std::to_string(x.osm_relation_id)+"\" "
					// 		"and OSM from-way \""+std::to_string(routing_way.to_global(x.from_way))+"\" "
					// 		"and OSM to-way \""+std::to_string(routing_way.to_global(x.to_way))+"\" "
					// 		"and OSM via-node \""+std::to_string(routing_node.to_global(x.via_node))+"\""
					// 		", ignoring restriction"
					// 	);
					// }
					continue;
				}
				#ifndef NDEBUG
				for(auto to:to_candidates)
					assert(tail[to] == x.via_node);
				#endif


				unsigned from, to;
				if(from_candidates.size() == 1 && to_candidates.size() == 1){
					from = from_candidates[0];
					to = to_candidates[0];
				} else {

					float via_lat = routing_graph.latitude[x.via_node];
					float via_lon = routing_graph.longitude[x.via_node];

					const float pi = 3.14159265359f;

					auto mod_2pi = [&](float angle) -> float {
						while(angle < 0.0f){
							angle += 2.0f*pi;
						}
						while(angle > 2.0f*pi){
							angle -= 2.0f*pi;
						}
						return angle;
					};

					unsigned matching_candidate_count = 0;

					for(unsigned from_cand: from_candidates){

						float from_lat, from_lon;
						if(routing_graph.first_modelling_node[from_cand] == routing_graph.first_modelling_node[from_cand+1]){
							from_lat = routing_graph.latitude[tail[from_cand]];
							from_lon = routing_graph.longitude[tail[from_cand]];
						}else{
							from_lat = routing_graph.modelling_node_latitude[routing_graph.first_modelling_node[from_cand+1]-1];
							from_lon = routing_graph.modelling_node_longitude[routing_graph.first_modelling_node[from_cand+1]-1];
						}

						float from_angle = atan2(via_lat-from_lat, via_lon-from_lon);

						for(unsigned to_cand: to_candidates){
							float to_lat, to_lon;

							if(routing_graph.first_modelling_node[to_cand] == routing_graph.first_modelling_node[to_cand+1]){
								to_lat = routing_graph.latitude[routing_graph.head[to_cand]];
								to_lon = routing_graph.longitude[routing_graph.head[to_cand]];
							}else{
								to_lat = routing_graph.modelling_node_latitude[routing_graph.first_modelling_node[to_cand]];
								to_lon = routing_graph.modelling_node_longitude[routing_graph.first_modelling_node[to_cand]];
							}

							float to_angle = atan2(to_lat-via_lat, to_lon-via_lon);

							float angle_diff = mod_2pi(to_angle - from_angle);

							switch(x.direction){
								case OSMTurnDirection::left_turn:
								if(pi*1.0/4.0 < angle_diff && angle_diff < pi*3.0/4.0){
									++matching_candidate_count;
									from = from_cand;
									to = to_cand;
								}
								break;
								case OSMTurnDirection::right_turn:
								if(pi*5.0/4.0 < angle_diff && angle_diff < pi*7.0/4.0){
									++matching_candidate_count;
									from = from_cand;
									to = to_cand;
								}
								break;
								case OSMTurnDirection::straight_on:
								if(angle_diff < pi/3.0f || 5.0f*pi/3.0f < angle_diff){
									++matching_candidate_count;
									from = from_cand;
									to = to_cand;
								}
								break;
								case OSMTurnDirection::u_turn:
								if(2.0f*pi/3.0f < angle_diff && angle_diff < 4.0f*pi/3.0f){
									++matching_candidate_count;
									from = from_cand;
									to = to_cand;
								}
								break;
							}
						}
					}

					if(matching_candidate_count == 0){
						log_message(
							"OSM turn restriction relation ID \""+std::to_string(x.osm_relation_id)+"\" "
							"is a turn restriction where it is impossible to infer the restriction without "
							"using the turn direction information. However, no restriction candidate is consistent "
							"with the turn direction. -> ignoring restriction"
						);
						continue;
					}

					if(matching_candidate_count >= 2){
						log_message(
							"OSM turn restriction relation ID \""+std::to_string(x.osm_relation_id)+"\" "
							"is a turn restriction where it is impossible to infer the restriction without "
							"using the turn direction information. However, "+std::to_string(matching_candidate_count)+" restriction candidates are consistent "
							"with the turn direction -> ignoring all candidates restriction"
						);
						continue;
					}

				}

				if(x.category == OSMTurnRestrictionCategory::prohibitive){
					add_forbidden_turn(from, to);
				} else {
					for(unsigned i=first_out[x.via_node]; i!=first_out[x.via_node+1]; ++i){
						if(i != to){
							add_forbidden_turn(from, i);
						}
					}
				}
			}

			if(log_message){
				timer += get_micro_time();
				log_message("Finished, needed "+std::to_string(timer)+" musec.");
			}
		}

		{
			if(log_message){
				log_message("Sorting forbidden turns");
				timer = -get_micro_time();
			}

			auto p = compute_inverse_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(arc_count, forbidden_from, forbidden_to);
			forbidden_to = apply_inverse_permutation(p, move(forbidden_to));

			assert(is_sorted_using_less(forbidden_from));

			BitVector is_duplicate = make_bit_vector(
				forbidden_from.size(),
				[&](unsigned x){
					if(x == 0)
						return false;
					else
						return forbidden_from[x-1] == forbidden_from[x] && forbidden_to[x-1] == forbidden_to[x];
				}
			);
			inplace_remove_element_from_vector_if(is_duplicate, forbidden_from);
			inplace_remove_element_from_vector_if(is_duplicate, forbidden_to);

			assert(is_sorted_using_less(forbidden_from));

			if(log_message){
				timer += get_micro_time();
				log_message("Finished, needed "+std::to_string(timer)+" musec.");
			}
		}

		routing_graph.forbidden_turn_from_arc = std::move(forbidden_from);
		routing_graph.forbidden_turn_to_arc = std::move(forbidden_to);
	}

	if(log_message){
		log_message("Extracted "+std::to_string(routing_graph.forbidden_turn_from_arc.size())+" forbidden turns.");
	}

	return routing_graph; // NVRO
}

} // RoutingKit


