#include <routingkit/contraction_hierarchy.h>
#include <routingkit/customizable_contraction_hierarchy.h>
#include <routingkit/osm_graph_builder.h>
#include <routingkit/osm_profile.h>
#include <routingkit/nested_dissection.h>
#include <routingkit/timer.h>

#include "expect.h"

#include <iostream>
#include <random>


using namespace std;
using namespace RoutingKit;

int main(int argc, char*argv[]){

	string pbf_file;

	unsigned test_count = 100;

	if(argc == 2){
		pbf_file = argv[1];
	} else if(argc == 3) {
		pbf_file = argv[1];
		test_count = atoi(argv[2]);
	} else {
		cout
			<< "Usage: "<< argv[0] << " test.pbf [test_count]\n"
			<< "Example: \n"
			<< "wget https://download.geofabrik.de/europe/germany/baden-wuerttemberg-latest.osm.pbf\n"
			<< argv[0] << " baden-wuerttemberg-latest.osm.pbf" << endl;
		return 1;
	}

	auto log_message = [](const string&msg){
		cout << msg << endl;
	};

	vector<unsigned>first_out;
	vector<unsigned>tail;
	vector<unsigned>head;
	vector<unsigned>travel_time; // in seconds
	vector<float>latitude;
	vector<float>longitude;
	
	{
		auto mapping = load_osm_id_mapping_from_pbf(pbf_file,
			nullptr,
			[&](uint64_t osm_way_id, const TagMap&tags){
				return is_osm_way_used_by_cars(osm_way_id, tags, log_message);
			},
			log_message
		);

		vector<uint32_t>way_speed(mapping.is_routing_way.population_count()); // in km/h

		auto routing_graph = load_osm_routing_graph_from_pbf(
			pbf_file,
			mapping,
			[&](uint64_t osm_way_id, unsigned routing_way_id, const TagMap&way_tags){
				way_speed[routing_way_id] = get_osm_way_speed(osm_way_id, way_tags, log_message);
				return get_osm_car_direction_category(osm_way_id, way_tags, log_message);
			},
			nullptr,
			log_message,
			false
		);

		first_out = move(routing_graph.first_out);
		tail = invert_inverse_vector(first_out);
		head = move(routing_graph.head);
		latitude = move(routing_graph.latitude);
		longitude = move(routing_graph.longitude);
		travel_time = move(routing_graph.geo_distance);

		// routing_graph.geo_distance is in meter

		for(unsigned a=0; a<travel_time.size(); ++a){
			travel_time[a] *= 18;
			travel_time[a] /= way_speed[routing_graph.way[a]];
			travel_time[a] /= 5;
		}


	}

	unsigned node_count = first_out.size()-1;
	unsigned arc_count = head.size();

	vector<unsigned>source1(test_count);
	vector<unsigned>source2(test_count);
	vector<unsigned>source3(test_count);
	vector<unsigned>target1(test_count);
	vector<unsigned>target2(test_count);
	vector<unsigned>target3(test_count);

	mt19937 rand_gen;
	rand_gen.seed(42);

	{
		std::uniform_int_distribution<> dist(0, node_count-1);
		
		for(auto&x:source1)
			x = dist(rand_gen);
		for(auto&x:source2)
			x = dist(rand_gen);
		for(auto&x:source3)
			x = dist(rand_gen);
		for(auto&x:target1)
			x = dist(rand_gen);
		for(auto&x:target2)
			x = dist(rand_gen);
		for(auto&x:target3)
			x = dist(rand_gen);
	}

	vector<unsigned>shortest_path_distance1(test_count);
	vector<unsigned>shortest_path_distance2(test_count);
	vector<unsigned>shortest_path_distance3(test_count);

	auto check_result = [&](unsigned used_source, unsigned used_target, unsigned shortest_path_length, const vector<unsigned>&node_path, const vector<unsigned>&arc_path){
		if(shortest_path_length == inf_weight){
			EXPECT(node_path.empty());
			EXPECT(arc_path.empty());
		} else if(node_path.size() == 1) {
			EXPECT_CMP(shortest_path_length, ==, 0);
			EXPECT_CMP(arc_path.size(), ==, 0);
			EXPECT_CMP(used_source, ==, used_target);
			EXPECT_CMP(used_source, ==, node_path[0]);
		} else {
			// Are IDs valid?
			EXPECT_CMP(max_element_of(node_path), <, node_count);
			EXPECT_CMP(max_element_of(arc_path), <, arc_count);

			// Is a valid path?
			EXPECT_CMP(node_path.size(), ==, arc_path.size()+1);
			for(unsigned i=0; i<arc_path.size(); ++i){
				EXPECT_CMP(tail[arc_path[i]], ==, node_path[i]);
				EXPECT_CMP(head[arc_path[i]], ==, node_path[i+1]);
			}

			// Is a st-path?
			EXPECT_CMP(node_path.front(), ==, used_source);
			EXPECT_CMP(node_path.back(), ==, used_target);

			// Has the claimed length?
			unsigned actual_path_length = 0;
			for(auto a:arc_path)
				actual_path_length += travel_time[a];
			EXPECT_CMP(shortest_path_length, ==, actual_path_length);
		}
	};

	ContractionHierarchy ch = ContractionHierarchy::build(node_count, tail, head, travel_time, log_message);
	auto cch_order = compute_nested_node_dissection_order_using_inertial_flow(node_count, tail, head, latitude, longitude, log_message);
		
	{
		ContractionHierarchyQuery q(ch);
		{
			long long ch_one_to_one_query_time = 0;
			for(unsigned i=0; i<test_count; ++i){
				ch_one_to_one_query_time -= get_micro_time();
				q
					.reset()
					.add_source(source1[i])
					.add_target(target1[i])
					.run();
				ch_one_to_one_query_time += get_micro_time();
				shortest_path_distance1[i] = q.get_distance();

				if(q.get_distance() != inf_weight){
					EXPECT_CMP(q.get_used_source(), ==, source1[i]);
					EXPECT_CMP(q.get_used_target(), ==, target1[i]);

					check_result(q.get_used_source(), q.get_used_target(), q.get_distance(), q.get_node_path(), q.get_arc_path());
				}else{
					EXPECT(q.get_arc_path().empty());
					EXPECT(q.get_node_path().empty());

					EXPECT_CMP(q.get_used_source(), ==, invalid_id);
					EXPECT_CMP(q.get_used_target(), ==, invalid_id);
				}
			}
			log_message("CH one-to-one distance query running time average over "+to_string(test_count)+" uniform random queries : "+to_string(ch_one_to_one_query_time/test_count)+"musec");
			for(unsigned i=0; i<test_count; ++i){
				q
					.reset()
					.add_source(source1[i])
					.add_source(source2[i])
					.add_target(target1[i])
					.add_target(target2[i])
					.run();
				shortest_path_distance2[i] = q.get_distance();
				if(q.get_distance() != inf_weight){
					check_result(q.get_used_source(), q.get_used_target(), q.get_distance(), q.get_node_path(), q.get_arc_path());
				}else{
					EXPECT(q.get_arc_path().empty());
					EXPECT(q.get_node_path().empty());

					EXPECT_CMP(q.get_used_source(), ==, invalid_id);
					EXPECT_CMP(q.get_used_target(), ==, invalid_id);
				}
			}
			for(unsigned i=0; i<test_count; ++i){
				q
					.reset()
					.add_source(source1[i])
					.add_source(source2[i])
					.add_source(source3[i])
					.add_target(target1[i])
					.add_target(target2[i])
					.add_target(target3[i])
					.run();
				shortest_path_distance3[i] = q.get_distance();
				if(q.get_distance() != inf_weight){
					check_result(q.get_used_source(), q.get_used_target(), q.get_distance(), q.get_node_path(), q.get_arc_path());
				}else{
					EXPECT(q.get_arc_path().empty());
					EXPECT(q.get_node_path().empty());

					EXPECT_CMP(q.get_used_source(), ==, invalid_id);
					EXPECT_CMP(q.get_used_target(), ==, invalid_id);
				}
			}

		}
	}

	{
		CustomizableContractionHierarchy cch(cch_order, tail, head, log_message);
		CustomizableContractionHierarchyMetric m(cch, travel_time);
		long long customize_time = -get_micro_time();
		m.customize();
		customize_time += get_micro_time();
		log_message("CCH sequential customization running time : "+to_string(customize_time));
		
		CustomizableContractionHierarchyQuery q(m);
		{
			long long cch_one_to_one_query_time = 0;
			for(unsigned i=0; i<test_count; ++i){
				cch_one_to_one_query_time -= get_micro_time();
				q
					.reset()
					.add_source(source1[i])
					.add_target(target1[i])
					.run();
				cch_one_to_one_query_time += get_micro_time();
				
				EXPECT_CMP(shortest_path_distance1[i], ==, q.get_distance());
				if(q.get_distance() != inf_weight){
					EXPECT_CMP(q.get_used_source(), ==, source1[i]);
					EXPECT_CMP(q.get_used_target(), ==, target1[i]);

					check_result(q.get_used_source(), q.get_used_target(), q.get_distance(), q.get_node_path(), q.get_arc_path());
				}else{
					EXPECT(q.get_arc_path().empty());
					EXPECT(q.get_node_path().empty());

					EXPECT_CMP(q.get_used_source(), ==, invalid_id);
					EXPECT_CMP(q.get_used_target(), ==, invalid_id);
				}
			}
			log_message("CCH one-to-one distance query running time average over "+to_string(test_count)+" uniform random queries : "+to_string(cch_one_to_one_query_time/test_count)+"musec");
			for(unsigned i=0; i<test_count; ++i){
				q
					.reset()
					.add_source(source1[i])
					.add_source(source2[i])
					.add_target(target1[i])
					.add_target(target2[i])
					.run();

				EXPECT_CMP(shortest_path_distance2[i], ==, q.get_distance());
				if(q.get_distance() != inf_weight){
					check_result(q.get_used_source(), q.get_used_target(), q.get_distance(), q.get_node_path(), q.get_arc_path());
				}else{
					EXPECT(q.get_arc_path().empty());
					EXPECT(q.get_node_path().empty());

					EXPECT_CMP(q.get_used_source(), ==, invalid_id);
					EXPECT_CMP(q.get_used_target(), ==, invalid_id);
				}
			}
			for(unsigned i=0; i<test_count; ++i){
				q
					.reset()
					.add_source(source1[i])
					.add_source(source2[i])
					.add_source(source3[i])
					.add_target(target1[i])
					.add_target(target2[i])
					.add_target(target3[i])
					.run();
				EXPECT_CMP(shortest_path_distance3[i], ==, q.get_distance());
				check_result(q.get_used_source(), q.get_used_target(), q.get_distance(), q.get_node_path(), q.get_arc_path());
			}
		}
	}

	{
		CustomizableContractionHierarchy cch(cch_order, tail, head);
		CustomizableContractionHierarchyMetric m(cch, travel_time);
		auto perfect_ch = m.build_contraction_hierarchy_using_perfect_witness_search();
		
		cout << "perfect CH has " << perfect_ch.forward.head.size() << " forward arcs " << endl;
		cout << "perfect CH has " << perfect_ch.backward.head.size() << " backward arcs " << endl;

		ContractionHierarchyQuery q(perfect_ch);
		{
			long long cch_perfect_one_to_one_query_time = 0;
			for(unsigned i=0; i<test_count; ++i){
				cch_perfect_one_to_one_query_time -= get_micro_time();
				q
					.reset()
					.add_source(source1[i])
					.add_target(target1[i])
					.run();
				EXPECT_CMP(shortest_path_distance1[i], ==, q.get_distance());
				cch_perfect_one_to_one_query_time += get_micro_time();
				
				if(q.get_distance() != inf_weight){
					EXPECT_CMP(q.get_used_source(), ==, source1[i]);
					EXPECT_CMP(q.get_used_target(), ==, target1[i]);

					check_result(q.get_used_source(), q.get_used_target(), q.get_distance(), q.get_node_path(), q.get_arc_path());
				}else{
					EXPECT(q.get_arc_path().empty());
					EXPECT(q.get_node_path().empty());

					EXPECT_CMP(q.get_used_source(), ==, invalid_id);
					EXPECT_CMP(q.get_used_target(), ==, invalid_id);
				}
			}
			log_message("Perfectly customized CCH one-to-one distance query running time average over "+to_string(test_count)+" uniform random queries : "+to_string(cch_perfect_one_to_one_query_time/test_count)+"musec");
			for(unsigned i=0; i<test_count; ++i){
				q
					.reset()
					.add_source(source1[i])
					.add_source(source2[i])
					.add_target(target1[i])
					.add_target(target2[i])
					.run();
				EXPECT_CMP(shortest_path_distance2[i], ==, q.get_distance());

				if(q.get_distance() != inf_weight){
					check_result(q.get_used_source(), q.get_used_target(), q.get_distance(), q.get_node_path(), q.get_arc_path());
				}else{
					EXPECT(q.get_arc_path().empty());
					EXPECT(q.get_node_path().empty());

					EXPECT_CMP(q.get_used_source(), ==, invalid_id);
					EXPECT_CMP(q.get_used_target(), ==, invalid_id);
				}
			}
			for(unsigned i=0; i<test_count; ++i){
				q
					.reset()
					.add_source(source1[i])
					.add_source(source2[i])
					.add_source(source3[i])
					.add_target(target1[i])
					.add_target(target2[i])
					.add_target(target3[i])
					.run();
				EXPECT_CMP(shortest_path_distance3[i], ==, q.get_distance());

				if(q.get_distance() != inf_weight){
					check_result(q.get_used_source(), q.get_used_target(), q.get_distance(), q.get_node_path(), q.get_arc_path());
				}else{
					EXPECT(q.get_arc_path().empty());
					EXPECT(q.get_node_path().empty());

					EXPECT_CMP(q.get_used_source(), ==, invalid_id);
					EXPECT_CMP(q.get_used_target(), ==, invalid_id);
				}
			}

		}

	}

	vector<unsigned>source1_offset(test_count);
	vector<unsigned>source2_offset(test_count);
	vector<unsigned>source3_offset(test_count);
	vector<unsigned>target1_offset(test_count);
	vector<unsigned>target2_offset(test_count);
	vector<unsigned>target3_offset(test_count);

	{
		std::uniform_int_distribution<> dist(0, 100);
		
		for(auto&x:source1_offset)
			x = dist(rand_gen);
		for(auto&x:source2_offset)
			x = dist(rand_gen);
		for(auto&x:source3_offset)
			x = dist(rand_gen);
		for(auto&x:target1_offset)
			x = dist(rand_gen);
		for(auto&x:target2_offset)
			x = dist(rand_gen);
		for(auto&x:target3_offset)
			x = dist(rand_gen);
	}

	{
		ContractionHierarchyQuery q(ch);
		{
			for(unsigned i=0; i<test_count; ++i){
				q
					.reset()
					.add_source(source1[i], source1_offset[i])
					.add_target(target1[i], target1_offset[i])
					.run();
				shortest_path_distance1[i] = q.get_distance();

				if(q.get_distance() != inf_weight){
					EXPECT_CMP(q.get_used_source(), ==, source1[i]);
					EXPECT_CMP(q.get_used_target(), ==, target1[i]);

					check_result(q.get_used_source(), q.get_used_target(), q.get_distance()-source1_offset[i]-target1_offset[i], q.get_node_path(), q.get_arc_path());
				}else{
					EXPECT(q.get_arc_path().empty());
					EXPECT(q.get_node_path().empty());

					EXPECT_CMP(q.get_used_source(), ==, invalid_id);
					EXPECT_CMP(q.get_used_target(), ==, invalid_id);
				}
			}
			for(unsigned i=0; i<test_count; ++i){
				q
					.reset()
					.add_source(source1[i], source1_offset[i])
					.add_source(source2[i], source2_offset[i])
					.add_target(target1[i], target1_offset[i])
					.add_target(target2[i], target2_offset[i])
					.run();
				shortest_path_distance2[i] = q.get_distance();
				if(q.get_distance() != inf_weight){
					bool use_source1 = true;

					if(source1[i] == source2[i])
						use_source1 = source1_offset[i] <= source2_offset[i];
					else if(source2[i] == q.get_used_source())
						use_source1 = false;
					else
						EXPECT_CMP(source1[i], ==, q.get_used_source());

					bool use_target1 = true;

					if(target1[i] == target2[i])
						use_target1 = target1_offset[i] <= target2_offset[i];
					else if(target2[i] == q.get_used_target())
						use_target1 = false;
					else
						EXPECT_CMP(target1[i], ==, q.get_used_target());

					if(use_source1 && use_target1)
						check_result(q.get_used_source(), q.get_used_target(), q.get_distance()-source1_offset[i]-target1_offset[i], q.get_node_path(), q.get_arc_path());
					else if(use_source1 && !use_target1)
						check_result(q.get_used_source(), q.get_used_target(), q.get_distance()-source1_offset[i]-target2_offset[i], q.get_node_path(), q.get_arc_path());
					else if(!use_source1 && use_target1)
						check_result(q.get_used_source(), q.get_used_target(), q.get_distance()-source2_offset[i]-target1_offset[i], q.get_node_path(), q.get_arc_path());
					else if(!use_source1 && !use_target1)
						check_result(q.get_used_source(), q.get_used_target(), q.get_distance()-source2_offset[i]-target2_offset[i], q.get_node_path(), q.get_arc_path());
				}else{
					EXPECT(q.get_arc_path().empty());
					EXPECT(q.get_node_path().empty());

					EXPECT_CMP(q.get_used_source(), ==, invalid_id);
					EXPECT_CMP(q.get_used_target(), ==, invalid_id);
				}
			}
		}
	}

	{
		CustomizableContractionHierarchy cch(cch_order, tail, head, log_message);
		CustomizableContractionHierarchyMetric m(cch, travel_time);
		m.customize();
		CustomizableContractionHierarchyQuery q(m);
		{
			for(unsigned i=0; i<test_count; ++i){
				q
					.reset()
					.add_source(source1[i], source1_offset[i])
					.add_target(target1[i], target1_offset[i])
					.run();
				shortest_path_distance1[i] = q.get_distance();

				if(q.get_distance() != inf_weight){
					EXPECT_CMP(q.get_used_source(), ==, source1[i]);
					EXPECT_CMP(q.get_used_target(), ==, target1[i]);

					check_result(q.get_used_source(), q.get_used_target(), q.get_distance()-source1_offset[i]-target1_offset[i], q.get_node_path(), q.get_arc_path());
				}else{
					EXPECT(q.get_arc_path().empty());
					EXPECT(q.get_node_path().empty());

					EXPECT_CMP(q.get_used_source(), ==, invalid_id);
					EXPECT_CMP(q.get_used_target(), ==, invalid_id);
				}
			}
			for(unsigned i=0; i<test_count; ++i){
				q
					.reset()
					.add_source(source1[i], source1_offset[i])
					.add_source(source2[i], source2_offset[i])
					.add_target(target1[i], target1_offset[i])
					.add_target(target2[i], target2_offset[i])
					.run();
				shortest_path_distance2[i] = q.get_distance();
				if(q.get_distance() != inf_weight){
					bool use_source1 = true;

					if(source1[i] == source2[i])
						use_source1 = source1_offset[i] <= source2_offset[i];
					else if(source2[i] == q.get_used_source())
						use_source1 = false;
					else
						EXPECT_CMP(source1[i], ==, q.get_used_source());

					bool use_target1 = true;

					if(target1[i] == target2[i])
						use_target1 = target1_offset[i] <= target2_offset[i];
					else if(target2[i] == q.get_used_target())
						use_target1 = false;
					else
						EXPECT_CMP(target1[i], ==, q.get_used_target());

					if(use_source1 && use_target1)
						check_result(q.get_used_source(), q.get_used_target(), q.get_distance()-source1_offset[i]-target1_offset[i], q.get_node_path(), q.get_arc_path());
					else if(use_source1 && !use_target1)
						check_result(q.get_used_source(), q.get_used_target(), q.get_distance()-source1_offset[i]-target2_offset[i], q.get_node_path(), q.get_arc_path());
					else if(!use_source1 && use_target1)
						check_result(q.get_used_source(), q.get_used_target(), q.get_distance()-source2_offset[i]-target1_offset[i], q.get_node_path(), q.get_arc_path());
					else if(!use_source1 && !use_target1)
						check_result(q.get_used_source(), q.get_used_target(), q.get_distance()-source2_offset[i]-target2_offset[i], q.get_node_path(), q.get_arc_path());
				}else{
					EXPECT(q.get_arc_path().empty());
					EXPECT(q.get_node_path().empty());

					EXPECT_CMP(q.get_used_source(), ==, invalid_id);
					EXPECT_CMP(q.get_used_target(), ==, invalid_id);
				}
			}
		}
	}


	{
		ContractionHierarchyQuery ch_query(ch);
		ContractionHierarchyQuery ref_query(ch);

		CustomizableContractionHierarchy cch(cch_order, tail, head);
		CustomizableContractionHierarchyMetric m(cch, travel_time);
		m.customize();
		CustomizableContractionHierarchyQuery cch_query(m);
		CustomizableContractionHierarchyQuery cch_ref_query(m);
		
		long long ch_pin_time = -get_micro_time();
		ch_query
			.reset()
			.pin_targets(target1);
		ch_pin_time += get_micro_time();

		long long cch_pin_time = -get_micro_time();
		cch_query
			.reset()
			.pin_targets(target1);
		cch_pin_time += get_micro_time();

		log_message("CH pin targets running time with "+to_string(target1.size())+" targets : "+to_string(ch_pin_time)+"musec");
		log_message("CCH pin targets running time with "+to_string(target1.size())+" targets : "+to_string(ch_pin_time)+"musec");

		long long ch_one_to_many_time = 0, cch_one_to_many_time = 0, ch_naive_one_to_many = 0, cch_naive_one_to_many = 0;
		for(unsigned i=0; i<test_count; ++i){
			ch_one_to_many_time -= get_micro_time();
			ch_query
				.reset_source()
				.add_source(source1[i], source1_offset[i])
				.run_to_pinned_targets();
			ch_one_to_many_time += get_micro_time();
			cch_one_to_many_time -= get_micro_time();
			cch_query
				.reset_source()
				.add_source(source1[i], source1_offset[i])
				.run_to_pinned_targets();
			cch_one_to_many_time += get_micro_time();
			
			auto d = ch_query.get_distances_to_targets();
			for(unsigned j=0; j<test_count; ++j){
				ch_naive_one_to_many -= get_micro_time();
				EXPECT_CMP(
					ref_query
						.reset()
						.add_source(source1[i], source1_offset[i])
						.add_target(target1[j])
						.run()
						.get_distance(), ==, d[j]);
				ch_naive_one_to_many += get_micro_time();
				cch_naive_one_to_many -= get_micro_time();
				EXPECT_CMP(
					cch_ref_query
						.reset()
						.add_source(source1[i], source1_offset[i])
						.add_target(target1[j])
						.run()
						.get_distance(), ==, d[j]);
				cch_naive_one_to_many += get_micro_time();
			}

			EXPECT(d == cch_query.get_distances_to_targets());

		}

		log_message("Naive CH pinned 1-to-target-query running time averaged over "+to_string(test_count)+" runs : "+to_string(ch_naive_one_to_many/test_count)+"musec");
		log_message("Naive CCH pinned 1-to-target-query running time averaged over "+to_string(test_count)+" runs : "+to_string(cch_naive_one_to_many/test_count)+"musec");
		log_message("CH pinned 1-to-target-query running time averaged over "+to_string(test_count)+" runs : "+to_string(ch_one_to_many_time/test_count)+"musec");
		log_message("CCH pinned 1-to-target-query running time averaged over "+to_string(test_count)+" runs : "+to_string(cch_one_to_many_time/test_count)+"musec");
		
	}

	{
		ContractionHierarchyQuery ch_query(ch);
		ContractionHierarchyQuery ref_query(ch);

		CustomizableContractionHierarchy cch(cch_order, tail, head);
		CustomizableContractionHierarchyMetric m(cch, travel_time);
		m.customize();
		CustomizableContractionHierarchyQuery cch_query(m);
	
		long long ch_pin_time = -get_micro_time();
		ch_query
			.reset()
			.pin_targets(target1);
		ch_pin_time += get_micro_time();

		long long cch_pin_time = -get_micro_time();
		cch_query
			.reset()
			.pin_targets(target1);
		cch_pin_time += get_micro_time();

		log_message("CH pin targets running time with "+to_string(target1.size())+" targets : "+to_string(ch_pin_time)+"musec");
		log_message("CCH pin targets running time with "+to_string(target1.size())+" targets : "+to_string(ch_pin_time)+"musec");

		long long ch_one_to_many_time = 0, cch_one_to_many_time = 0, ch_naive_one_to_many = 0;
		for(unsigned i=0; i<test_count; ++i){
			ch_one_to_many_time -= get_micro_time();
			ch_query
				.reset_source()
				.add_source(source1[i], source1_offset[i])
				.add_source(source2[i], source2_offset[i])
				.add_source(source3[i], source3_offset[i])
				.run_to_pinned_targets();
			ch_one_to_many_time += get_micro_time();
			cch_one_to_many_time -= get_micro_time();
			cch_query
				.reset_source()
				.add_source(source1[i], source1_offset[i])
				.add_source(source2[i], source2_offset[i])
				.add_source(source3[i], source3_offset[i])
				.run_to_pinned_targets();
			cch_one_to_many_time += get_micro_time();
			
			auto d = ch_query.get_distances_to_targets();
			for(unsigned j=0; j<test_count; ++j){
				ch_naive_one_to_many -= get_micro_time();
				EXPECT_CMP(
					ref_query
						.reset()
						.add_source(source1[i], source1_offset[i])
						.add_source(source2[i], source2_offset[i])
						.add_source(source3[i], source3_offset[i])
						.add_target(target1[j])
						.run()
						.get_distance(), ==, d[j]);
				ch_naive_one_to_many += get_micro_time();
			}

			EXPECT(d == cch_query.get_distances_to_targets());

		}

		log_message("Naive CH pinned 3-to-target-query running time averaged over "+to_string(test_count)+" runs : "+to_string(ch_naive_one_to_many/test_count)+"musec");
		log_message("CH 3-to-target-query running time averaged over "+to_string(test_count)+" runs : "+to_string(ch_one_to_many_time/test_count)+"musec");
		log_message("CCH 3-to-target-query running time averaged over "+to_string(test_count)+" runs : "+to_string(cch_one_to_many_time/test_count)+"musec");
		
	}

	{
		ContractionHierarchyQuery ch_query(ch);
		ContractionHierarchyQuery ref_query(ch);

		CustomizableContractionHierarchy cch(cch_order, tail, head);
		CustomizableContractionHierarchyMetric m(cch, travel_time);
		m.customize();
		CustomizableContractionHierarchyQuery cch_query(m);
	
		ch_query
			.reset()
			.pin_sources(source1);
		cch_query
			.reset()
			.pin_sources(source1);
		for(unsigned i=0; i<test_count; ++i){
			ch_query
				.reset_target()
				.add_target(target1[i], target1_offset[i])
				.add_target(target2[i], target2_offset[i])
				.add_target(target3[i], target3_offset[i])
				.run_to_pinned_sources();
			cch_query
				.reset_target()
				.add_target(target1[i], target1_offset[i])
				.add_target(target2[i], target2_offset[i])
				.add_target(target3[i], target3_offset[i])
				.run_to_pinned_sources();

			auto d = ch_query.get_distances_to_sources();
			for(unsigned j=0; j<test_count; ++j){
				EXPECT_CMP(
					ref_query
						.reset()
						.add_target(target1[i], target1_offset[i])
						.add_target(target2[i], target2_offset[i])
						.add_target(target3[i], target3_offset[i])
						.add_source(source1[j])
						.run()
						.get_distance(), ==, d[j]);
			}
			EXPECT(d == cch_query.get_distances_to_sources());
		}
	}

	{
		std::vector<unsigned>weight = travel_time;
		CustomizableContractionHierarchy cch(cch_order, tail, head, log_message);

		CustomizableContractionHierarchyMetric metric1(cch, weight);
		CustomizableContractionHierarchyMetric metric2(cch, weight);
		CustomizableContractionHierarchyMetric metric3(cch, weight);

		metric1.customize();
		metric2.customize();


		CustomizableContractionHierarchyParallelization parallel(cch);
		parallel.customize(metric3, 4);

		CustomizableContractionHierarchyPartialCustomization partial_customize(cch);

		std::uniform_int_distribution<> random_arc(0, head.size()-1);
		std::uniform_int_distribution<> random_weight(0, 1000);
		for(unsigned i=0; i<head.size()/10; ++i){
			unsigned a = random_arc(rand_gen);
			weight[a] = random_weight(rand_gen);
			partial_customize.update_arc(a);
		}

		CustomizableContractionHierarchyMetric metric4(cch, weight);
		metric4.customize();

		metric1.customize();
		partial_customize.customize(metric2);
		parallel.customize(metric3, 4);
		
		EXPECT(metric1.forward == metric4.forward);
		EXPECT(metric1.backward == metric4.backward);

		EXPECT(metric2.forward == metric4.forward);
		EXPECT(metric2.backward == metric4.backward);

		EXPECT(metric3.forward == metric4.forward);
		EXPECT(metric3.backward == metric4.backward);
	}

	return expect_failed;
}
