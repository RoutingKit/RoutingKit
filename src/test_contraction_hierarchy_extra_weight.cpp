#include <routingkit/vector_io.h>
#include <routingkit/permutation.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/min_max.h>
#include <routingkit/timer.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <random>

#include "expect.h"

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		if(argc != 1 && argc != 5)
			cout << argv[0] << " first_out head optimized_weight extra_weight" << endl;

		auto path_link = [](vector<unsigned>l, vector<unsigned>r){
			l.insert(l.end(), r.begin(), r.end());
			return l;
		};

		auto scalar_link = SaturatedWeightAddition();

		auto log_message = [](const std::string&msg){
			cout << msg << endl;
		};


		{
			auto link = SaturatedWeightAddition();

			EXPECT_CMP(link(0u,0u), ==, 0u);
			EXPECT_CMP(link(1u,1u), ==, 2u);
			EXPECT_CMP(link(0u,1u), ==, 1u);
			EXPECT_CMP(link(1u,0u), ==, 1u);

			for(unsigned i=0; i<inf_weight; i+=10000000u){
				EXPECT_CMP(link(i,inf_weight), ==, inf_weight);
				EXPECT_CMP(link(inf_weight,i), ==, inf_weight);
			}
			EXPECT_CMP(link(inf_weight,inf_weight), ==, inf_weight);


			EXPECT_CMP(link(0,0), ==, 0);
			EXPECT_CMP(link(1,1), ==, 2);
			EXPECT_CMP(link(0,1), ==, 1);
			EXPECT_CMP(link(1,0), ==, 1);
			EXPECT_CMP(link(0,0), ==, 0);
			EXPECT_CMP(link(-1,-1), ==, -2);
			EXPECT_CMP(link(0,-1), ==, -1);
			EXPECT_CMP(link(-1,0), ==, -1);
			EXPECT_CMP(link(-1,1), ==, 0);
			
			// for(int i=-1000000000; i<(int)inf_weight-10000000; i+=10000000){
			// 	EXPECT_CMP(link(i,(int)inf_weight), ==, (int)inf_weight);
			// 	EXPECT_CMP(link((int)inf_weight,i), ==, (int)inf_weight);
			// }
			EXPECT_CMP(link((int)inf_weight,(int)inf_weight), ==, (int)inf_weight);

		}

		{
			std::vector<unsigned>
				tail = {0, 1},
				head = {2, 3},
				weight = {1, 1},
				target_list = {3, 2};
			unsigned node_count = 4;
			auto ch = ContractionHierarchy::build(node_count, tail, head, weight);
			ContractionHierarchyQuery q(ch);
			std::vector<unsigned> 
				used_sources = q.reset()
				.pin_targets(target_list)
				.add_source(0)
				.add_source(1)
				.run_to_pinned_targets()
				.get_used_sources_to_targets();
			EXPECT_CMP(used_sources[0], ==, 1);
			EXPECT_CMP(used_sources[1], ==, 0);
		}

		{
			unsigned node_count = 4;
			std::vector<unsigned>
				tail = {0, 0, 1, 2},
				head = {1, 2, 3, 3},
				weight = {1, 10, 1, 10},
				extra_weight1 = {10, 1, 10, 1};
			std::vector<int>
				extra_weight2 = {-10, -1, -10, -1};
			std::vector<std::string>
				extra_weight3 = {"foo", "bla", "bar", "hoo"};
			auto ch = ContractionHierarchy::build(node_count, tail, head, weight);
			ContractionHierarchyQuery q(ch);

			q.add_source(0).add_target(3).run();

			EXPECT_CMP(q.get_distance(), ==, 2);
			EXPECT_CMP(q.get_extra_weight_distance(extra_weight1, SaturatedWeightAddition()), ==, 20);
			EXPECT_CMP(q.get_extra_weight_distance(extra_weight2, SaturatedWeightAddition()), ==, -20);
			EXPECT_CMP(q.get_extra_weight_distance(extra_weight3, [](std::string l, std::string r){return l+r;}), ==, "foobar");

			ContractionHierarchyExtraWeight<unsigned>extra_weight4(ch, extra_weight1, SaturatedWeightAddition());
			EXPECT_CMP(q.get_extra_weight_distance(extra_weight4, SaturatedWeightAddition()), ==, 20);

		}

		{
			unsigned node_count = 4;
			std::vector<unsigned>
				tail = {0, 0, 1, 2},
				head = {1, 2, 3, 3},
				weight = {1, 10, 1, 10},
				target_list = {3, 1};
			std::vector<std::string>
				extra_weight = {"foo", "bla", "bar", "hoo"};

			auto ch = ContractionHierarchy::build(node_count, tail, head, weight);
			ContractionHierarchyQuery q(ch);

			std::vector<std::string>d = q
				.pin_targets(target_list).add_source(0)
				.run_to_pinned_targets()
				.get_extra_weight_distances_to_targets(
					extra_weight, 
					[](std::string l, std::string r){return l+r;}
				)
			;
			EXPECT_CMP(d[0], ==, "foobar");
			EXPECT_CMP(d[1], ==, "foo");
		}

		{
			const unsigned node_count = 8;

			vector<unsigned>
				tail = {
					0,
					1, 1, 1,
					2,
					3,
					4, 4, 4,
					5,
					6, 6, 6,
					7, 7, 7
				},
				head = {
					4,
					5, 6, 7,
					6,
					7,
					0, 6, 7,
					1,
					1, 2, 4,
					1, 3, 4
				},
				weight = {
					2,
					2, 2, 2,
					1,
					1,
					2, 1, 1,
					1,
					1, 1, 1,
					1, 1, 1				
				},
				extra_scalar_weight = tail;

			const unsigned arc_count = tail.size();

			EXPECT_CMP(arc_count, == , head.size());
			EXPECT_CMP(arc_count, == , weight.size());

			vector<unsigned>order(node_count);
			for(unsigned i=0; i<node_count; ++i)
				order[i] = i;

			auto ch = ContractionHierarchy::build_given_order(order, tail, head, weight);

			vector<vector<unsigned>>extra_path_weight(arc_count);
			for(unsigned i=0; i<arc_count; ++i)
				extra_path_weight[i] = {i};

			ContractionHierarchyExtraWeight<vector<unsigned>>ch_extra_path_weight(ch, extra_path_weight, path_link);
			ContractionHierarchyExtraWeight<unsigned>ch_extra_scalar_weight(ch, extra_scalar_weight, scalar_link);

			ContractionHierarchyQuery q(ch);

			// test one-to-one

			for(unsigned s=0; s<node_count; ++s){
				for(unsigned t=0; t<node_count; ++t){

					q.reset().add_source(s).add_target(t).run();


					auto correct_path = q.get_arc_path();

					unsigned correct_extra_scalar_weight = 0;
					for(auto x:correct_path)
						correct_extra_scalar_weight += extra_scalar_weight[x];

					unsigned computed_extra_scalar_weight = q.get_extra_weight_distance(ch_extra_scalar_weight, scalar_link);

					EXPECT_CMP(correct_extra_scalar_weight, ==, computed_extra_scalar_weight);

					vector<unsigned> computed_extra_path_weight = q.get_extra_weight_distance(ch_extra_path_weight, path_link);
					EXPECT_CMP(computed_extra_path_weight.size(), ==, correct_path.size());
					// this is correct because multiple shortest paths are tie-broken using the same function
					EXPECT(computed_extra_path_weight == correct_path);
				}
			}

			// test many-to-many

			vector<unsigned>test_node_list(node_count);
			for(unsigned i=0; i<node_count; ++i)
				test_node_list[i] = node_count-i-1;

			ContractionHierarchyQuery p(ch);

			p.reset().pin_targets(test_node_list);
			for(unsigned s=0; s<node_count; ++s){

				p.reset_source().add_source(s).run_to_pinned_targets();

				auto scalar_result = p.get_extra_weight_distances_to_targets(ch_extra_scalar_weight, scalar_link);
				auto path_result = p.get_extra_weight_distances_to_targets(ch_extra_path_weight, path_link);

				p.reset_source().add_source(s).run_to_pinned_targets();

				auto scalar_result2 = p.get_extra_weight_distances_to_targets(ch_extra_scalar_weight, scalar_link);
				auto path_result2 = p.get_extra_weight_distances_to_targets(ch_extra_path_weight, path_link);
				EXPECT(scalar_result == scalar_result2);
				EXPECT(path_result == path_result2);

				for(unsigned i=0; i<node_count; ++i){
					unsigned t = test_node_list[i];
					q.reset().add_source(s).add_target(t).run();

					// Warning: path_result[i] is not necessarily equal to path_result[i] as shortest path
					// are not always unique.
					
					if(q.get_distance() == inf_weight){
						EXPECT_CMP(scalar_result[i], ==, 0);
						EXPECT(path_result[i].empty());	
					}else if(path_result[i].empty()){
						EXPECT_CMP(s, ==, t);
						EXPECT_CMP(scalar_result[i], ==, 0);
					}else{
						// Is the found path valid?

						EXPECT_CMP(tail[path_result[i].front()], ==, s);
						EXPECT_CMP(head[path_result[i].back()], ==, t);
						for(unsigned j=1; j<path_result[i].size(); ++j)
							EXPECT_CMP(head[path_result[i][j-1]], ==, tail[path_result[i][j]]);

						// Is it a shortest path?

						unsigned real_len = 0;
						for(auto x:path_result[i])
							real_len += weight[x];

						EXPECT_CMP(real_len, ==, q.get_distance());
					
						// Does it have the claimed scalar extra weight?

						unsigned real_extra_len = 0;
						for(auto x:path_result[i])
							real_extra_len += extra_scalar_weight[x];
					
						EXPECT_CMP(real_extra_len, ==, scalar_result[i]);
					}
				}
			}

			p.reset().pin_sources(test_node_list);
			for(unsigned t=0; t<node_count; ++t){
				p.reset_target().add_target(t).run_to_pinned_sources();

				auto scalar_result = p.get_extra_weight_distances_to_sources(ch_extra_scalar_weight, scalar_link);
				auto path_result = p.get_extra_weight_distances_to_sources(ch_extra_path_weight, path_link);

				p.reset_target().add_target(t).run_to_pinned_sources();

				auto scalar_result2 = p.get_extra_weight_distances_to_sources(ch_extra_scalar_weight, scalar_link);
				auto path_result2 = p.get_extra_weight_distances_to_sources(ch_extra_path_weight, path_link);
				EXPECT(scalar_result == scalar_result2);
				EXPECT(path_result == path_result2);

				for(unsigned i=0; i<node_count; ++i){
					unsigned s = test_node_list[i];
					q.reset().add_source(s).add_target(t).run();

					// Warning: path_result[i] is not necessarily equal to path_result[i] as shortest path
					// are not always unique.
					
					if(q.get_distance() == inf_weight){
						EXPECT_CMP(scalar_result[i], ==, 0);
						EXPECT(path_result[i].empty());	
					}else if(path_result[i].empty()){
						EXPECT_CMP(s, ==, t);
						EXPECT_CMP(scalar_result[i], ==, 0);
					}else{
						// Is the found path valid?

						EXPECT_CMP(tail[path_result[i].front()], ==, s);
						EXPECT_CMP(head[path_result[i].back()], ==, t);
						for(unsigned j=1; j<path_result[i].size(); ++j)
							EXPECT_CMP(head[path_result[i][j-1]], ==, tail[path_result[i][j]]);

						// Is it a shortest path?

						unsigned real_len = 0;
						for(auto x:path_result[i])
							real_len += weight[x];

						EXPECT_CMP(real_len, ==, q.get_distance());
					
						// Does it have the claimed scalar extra weight?

						unsigned real_extra_len = 0;
						for(auto x:path_result[i])
							real_extra_len += extra_scalar_weight[x];
					
						EXPECT_CMP(real_extra_len, ==, scalar_result[i]);
					}
				}
			}
		}



		if(argc == 5){

			vector<unsigned>
				first_out = load_vector<unsigned>(argv[1]),
				tail = invert_inverse_vector(first_out),
				head = load_vector<unsigned>(argv[2]),
				weight = load_vector<unsigned>(argv[3]),
				extra_scalar_weight = load_vector<unsigned>(argv[4]);

			const unsigned node_count = first_out.size()-1;
			const unsigned arc_count = tail.size();

			EXPECT_CMP(head.size(), ==, arc_count);
			EXPECT_CMP(weight.size(), ==, arc_count);
			EXPECT_CMP(extra_scalar_weight.size(), ==, arc_count);
			
			vector<vector<unsigned>>extra_path_weight(arc_count);
			for(unsigned i=0; i<arc_count; ++i)
				extra_path_weight[i] = {i};

			auto ch = ContractionHierarchy::build(node_count, tail, head, weight, log_message);

			long long scalar_weight_time = -get_micro_time();
			ContractionHierarchyExtraWeight<unsigned>ch_extra_scalar_weight(ch, extra_scalar_weight, scalar_link);
			scalar_weight_time += get_micro_time();

			cout << "time needed to compute scalar extra weight : " << scalar_weight_time << " musec" << endl;

			long long path_weight_time = -get_micro_time();
			ContractionHierarchyExtraWeight<vector<unsigned>>ch_extra_path_weight(ch, extra_path_weight, path_link);
			path_weight_time += get_micro_time();

			cout << "time needed to compute path extra weight : " << path_weight_time << " musec" << endl;

			unsigned long long total_size = 0;
			for(auto&xy:ch_extra_path_weight.forward_weight)
				total_size += xy.size();

			cout << "number of arcs in input : "<< arc_count << endl;
			cout << "number of arcs in forward CH : "<< ch.forward.head.size() << endl;
			cout << "number of arcs in forward extra path weight : "<< total_size << endl;

			const unsigned test_count = 10000;

			std::minstd_rand gen;
			gen.seed(42);
			std::uniform_int_distribution<unsigned>node_dist(0, node_count-1);

			ContractionHierarchyQuery q(ch);

			{
				for(unsigned t=0; t<test_count; ++t){
					unsigned source_node = node_dist(gen);
					unsigned target_node = node_dist(gen);
					
					auto p = q.reset().add_source(source_node).add_target(target_node).run().get_arc_path();

					unsigned correct_extra_scalar_weight = 0;
					for(auto x:p)
						correct_extra_scalar_weight += extra_scalar_weight[x];

					auto computed_extra_scalar_weight = q.get_extra_weight_distance(ch_extra_scalar_weight, scalar_link);
					EXPECT_CMP(correct_extra_scalar_weight, ==, computed_extra_scalar_weight);
					
					auto computed_extra_path_weight = q.get_extra_weight_distance(ch_extra_path_weight, path_link);
					EXPECT_CMP(computed_extra_path_weight.size(), ==, p.size());
					EXPECT(computed_extra_path_weight == p);


					auto computed_extra_scalar_weight2 = q.get_extra_weight_distance(extra_scalar_weight, scalar_link);
					EXPECT_CMP(correct_extra_scalar_weight, ==, computed_extra_scalar_weight2);
					
					auto computed_extra_path_weight2 = q.get_extra_weight_distance(extra_path_weight, path_link);
					EXPECT_CMP(computed_extra_path_weight2.size(), ==, p.size());
					EXPECT(computed_extra_path_weight2 == p);
				}

				cout << "Finished "<<test_count <<" one-to-one tests" << endl;
			}


	
			const unsigned target_test_count = 300;
	
			vector<unsigned>test_node_list(target_test_count);
			for(unsigned i=0; i<target_test_count; ++i)
				test_node_list[i] = node_dist(gen);

			ContractionHierarchyQuery p(ch);

			{
				p.reset().pin_targets(test_node_list);
				for(unsigned k=0; k<target_test_count; ++k){
					unsigned s = test_node_list[k];

					p.reset_source().add_source(s).run_to_pinned_targets();

					auto scalar_result = p.get_extra_weight_distances_to_targets(ch_extra_scalar_weight, scalar_link);
					auto path_result = p.get_extra_weight_distances_to_targets(ch_extra_path_weight, path_link);

					p.reset_source().add_source(s).run_to_pinned_targets();

					auto scalar_result2 = p.get_extra_weight_distances_to_targets(ch_extra_scalar_weight, scalar_link);
					auto path_result2 = p.get_extra_weight_distances_to_targets(ch_extra_path_weight, path_link);
					EXPECT(scalar_result == scalar_result2);
					EXPECT(path_result == path_result2);

					for(unsigned i=0; i<target_test_count; ++i){
						unsigned t = test_node_list[i];
						q.reset().add_source(s).add_target(t).run();

						// Warning: path_result[i] is not necessarily equal to path_result[i] as shortest path
						// are not always unique.
						
						if(q.get_distance() == inf_weight){
							EXPECT_CMP(scalar_result[i], ==, 0);
							EXPECT(path_result[i].empty());	
						}else if(path_result[i].empty()){
							EXPECT_CMP(s, ==, t);
							EXPECT_CMP(scalar_result[i], ==, 0);
						}else{
							// Is the found path valid?

							EXPECT_CMP(tail[path_result[i].front()], ==, s);
							EXPECT_CMP(head[path_result[i].back()], ==, t);
							for(unsigned j=1; j<path_result[i].size(); ++j)
								EXPECT_CMP(head[path_result[i][j-1]], ==, tail[path_result[i][j]]);

							// Is it a shortest path?

							unsigned real_len = 0;
							for(auto x:path_result[i])
								real_len += weight[x];

							EXPECT_CMP(real_len, ==, q.get_distance());
						
							// Does it have the claimed scalar extra weight?

							unsigned real_extra_len = 0;
							for(auto x:path_result[i])
								real_extra_len += extra_scalar_weight[x];
						
							EXPECT_CMP(real_extra_len, ==, scalar_result[i]);
						}
					}
				}

				cout << "Finished "<<target_test_count <<" one-to-"<<target_test_count <<" tests" << endl;
			}

			{
				p.reset().pin_sources(test_node_list);
				for(unsigned k=0; k<target_test_count; ++k){
					unsigned t = test_node_list[k];
					p.reset_target().add_target(t).run_to_pinned_sources();

					auto scalar_result = p.get_extra_weight_distances_to_sources(ch_extra_scalar_weight, scalar_link);
					auto path_result = p.get_extra_weight_distances_to_sources(ch_extra_path_weight, path_link);

					p.reset_target().add_target(t).run_to_pinned_sources();

					auto scalar_result2 = p.get_extra_weight_distances_to_sources(ch_extra_scalar_weight, scalar_link);
					auto path_result2 = p.get_extra_weight_distances_to_sources(ch_extra_path_weight, path_link);
					EXPECT(scalar_result == scalar_result2);
					EXPECT(path_result == path_result2);

					for(unsigned i=0; i<target_test_count; ++i){
						unsigned s = test_node_list[i];
						q.reset().add_source(s).add_target(t).run();

						// Warning: path_result[i] is not necessarily equal to path_result[i] as shortest path
						// are not always unique.
						
						if(q.get_distance() == inf_weight){
							EXPECT_CMP(scalar_result[i], ==, 0);
							EXPECT(path_result[i].empty());	
						}else if(path_result[i].empty()){
							EXPECT_CMP(s, ==, t);
							EXPECT_CMP(scalar_result[i], ==, 0);
						}else{
							// Is the found path valid?

							EXPECT_CMP(tail[path_result[i].front()], ==, s);
							EXPECT_CMP(head[path_result[i].back()], ==, t);
							for(unsigned j=1; j<path_result[i].size(); ++j)
								EXPECT_CMP(head[path_result[i][j-1]], ==, tail[path_result[i][j]]);

							// Is it a shortest path?

							unsigned real_len = 0;
							for(auto x:path_result[i])
								real_len += weight[x];

							EXPECT_CMP(real_len, ==, q.get_distance());
						
							// Does it have the claimed scalar extra weight?

							unsigned real_extra_len = 0;
							for(auto x:path_result[i])
								real_extra_len += extra_scalar_weight[x];
						
							EXPECT_CMP(real_extra_len, ==, scalar_result[i]);
						}
					}
				}

				cout << "Finished "<<target_test_count <<" "<<target_test_count <<"-to-one tests" << endl;

			}

			{
				p.reset().pin_targets(test_node_list);
				for(unsigned k=0; k<target_test_count-3; ++k){
					p
						.reset_source()
						.add_source(test_node_list[k+0])
						.add_source(test_node_list[k+1])
						.add_source(test_node_list[k+2])
						.run_to_pinned_targets();

					auto scalar_result = p.get_extra_weight_distances_to_targets(ch_extra_scalar_weight, scalar_link);
					auto path_result = p.get_extra_weight_distances_to_targets(ch_extra_path_weight, path_link);
					auto used_source = p.get_used_sources_to_targets();

					for(unsigned i=0; i<target_test_count; ++i){
						unsigned t = test_node_list[i];
						q
							.reset()
							.add_source(test_node_list[k+0])
							.add_source(test_node_list[k+1])
							.add_source(test_node_list[k+2])
							.add_target(t).run();

						if(q.get_distance() == inf_weight){
							EXPECT_CMP(scalar_result[i], ==, 0);
							EXPECT(path_result[i].empty());	
						}else if(path_result[i].empty()){
							EXPECT(test_node_list[k+0] == t || test_node_list[k+1] == t || test_node_list[k+2] == t);
							EXPECT_CMP(scalar_result[i], ==, 0);
						}else{
							EXPECT_CMP(tail[path_result[i].front()], ==, used_source[i]);
							EXPECT_CMP(head[path_result[i].back()], ==, t);
							for(unsigned j=1; j<path_result[i].size(); ++j)
								EXPECT_CMP(head[path_result[i][j-1]], ==, tail[path_result[i][j]]);

							unsigned real_len = 0;
							for(auto x:path_result[i])
								real_len += weight[x];

							EXPECT_CMP(real_len, ==, q.get_distance());
						
							// Does it have the claimed scalar extra weight?

							unsigned real_extra_len = 0;
							for(auto x:path_result[i])
								real_extra_len += extra_scalar_weight[x];
						
							EXPECT_CMP(real_extra_len, ==, scalar_result[i]);
						}
					}
				}

				cout << "Finished " << (target_test_count-3) <<" three-to-"<<target_test_count <<" tests" << endl;
			}

			{
				p.reset().pin_sources(test_node_list);
				for(unsigned k=0; k<target_test_count-3; ++k){
					p
						.reset_target()
						.add_target(test_node_list[k+0])
						.add_target(test_node_list[k+1])
						.add_target(test_node_list[k+2])
						.run_to_pinned_sources();

					auto scalar_result = p.get_extra_weight_distances_to_sources(ch_extra_scalar_weight, scalar_link);
					auto path_result = p.get_extra_weight_distances_to_sources(ch_extra_path_weight, path_link);
					auto used_target = p.get_used_targets_to_sources();

					for(unsigned i=0; i<target_test_count; ++i){
						unsigned s = test_node_list[i];
						q
							.reset()
							.add_target(test_node_list[k+0])
							.add_target(test_node_list[k+1])
							.add_target(test_node_list[k+2])
							.add_source(s).run();

						if(q.get_distance() == inf_weight){
							EXPECT_CMP(scalar_result[i], ==, 0);
							EXPECT(path_result[i].empty());	
						}else if(path_result[i].empty()){
							EXPECT(test_node_list[k+0] == s || test_node_list[k+1] == s || test_node_list[k+2] == s);
							EXPECT_CMP(scalar_result[i], ==, 0);
						}else{
							EXPECT_CMP(tail[path_result[i].front()], ==, s);
							EXPECT_CMP(head[path_result[i].back()], ==, used_target[i]);
							for(unsigned j=1; j<path_result[i].size(); ++j)
								EXPECT_CMP(head[path_result[i][j-1]], ==, tail[path_result[i][j]]);

							unsigned real_len = 0;
							for(auto x:path_result[i])
								real_len += weight[x];

							EXPECT_CMP(real_len, ==, q.get_distance());
						
							unsigned real_extra_len = 0;
							for(auto x:path_result[i])
								real_extra_len += extra_scalar_weight[x];
						
							EXPECT_CMP(real_extra_len, ==, scalar_result[i]);
						}
					}
				}

				cout << "Finished " << (target_test_count-3) << " " << target_test_count << "-to-three tests" << endl;
			}


			{
				auto weight_on_demand = detail::make_shortcut_weights(extra_scalar_weight, scalar_link, ch);
				for(unsigned i=0; i<ch_extra_scalar_weight.forward_weight.size(); ++i)
					EXPECT_CMP(ch_extra_scalar_weight.forward_weight[i], ==, weight_on_demand.get_forward_weight(i));
				for(unsigned i=0; i<ch_extra_scalar_weight.backward_weight.size(); ++i)
					EXPECT_CMP(ch_extra_scalar_weight.backward_weight[i], ==, weight_on_demand.get_backward_weight(i));
				
				cout << "Finished testing scalar ComputeShortcutWeightsOnDemand" << endl;
			}

			{
				auto weight_on_demand = detail::make_shortcut_weights(extra_path_weight, path_link, ch);
				for(unsigned i=0; i<ch_extra_path_weight.forward_weight.size(); ++i)
					EXPECT(ch_extra_path_weight.forward_weight[i] == weight_on_demand.get_forward_weight(i));
				for(unsigned i=0; i<ch_extra_path_weight.backward_weight.size(); ++i)
					EXPECT(ch_extra_path_weight.backward_weight[i] == weight_on_demand.get_backward_weight(i));
				
				cout << "Finished testing path ComputeShortcutWeightsOnDemand" << endl;
			}

			{
				p.reset().pin_targets(test_node_list);
				for(unsigned k=0; k<target_test_count; ++k){
					unsigned s = test_node_list[k];

					p.reset_source().add_source(s).run_to_pinned_targets();

					auto scalar_result = p.get_extra_weight_distances_to_targets(ch_extra_scalar_weight, scalar_link);
					auto path_result = p.get_extra_weight_distances_to_targets(ch_extra_path_weight, path_link);

					auto scalar_result2 = p.get_extra_weight_distances_to_targets(extra_scalar_weight, scalar_link);
					auto path_result2 = p.get_extra_weight_distances_to_targets(extra_path_weight, path_link);

					EXPECT(scalar_result == scalar_result2);
					EXPECT(path_result == path_result2);
				}
				cout << "Finished testing one-to-"<<target_test_count<<" with on-demand unpacking" << endl;
			}

			{
				p.reset().pin_sources(test_node_list);
				for(unsigned k=0; k<target_test_count; ++k){
					unsigned t = test_node_list[k];

					p.reset_target().add_target(t).run_to_pinned_sources();

					auto scalar_result = p.get_extra_weight_distances_to_sources(ch_extra_scalar_weight, scalar_link);
					auto path_result = p.get_extra_weight_distances_to_sources(ch_extra_path_weight, path_link);

					auto scalar_result2 = p.get_extra_weight_distances_to_sources(extra_scalar_weight, scalar_link);
					auto path_result2 = p.get_extra_weight_distances_to_sources(extra_path_weight, path_link);

					EXPECT(scalar_result == scalar_result2);
					EXPECT(path_result == path_result2);
				}
				cout << "Finished testing "<<target_test_count<<"-to-one with on-demand unpacking" << endl;
			}

			unsigned dummy = 0;

			{
				long long timer = 0;

				p.reset().pin_targets(test_node_list);
				for(unsigned k=0; k<target_test_count; ++k){
					p
						.reset_source()
						.add_source(test_node_list[k])
						.run_to_pinned_targets();

					timer -= get_micro_time();
					auto scalar_result = p.get_extra_weight_distances_to_targets(ch_extra_scalar_weight, scalar_link);
					timer += get_micro_time();
					
					for(auto x:scalar_result)
						dummy += x;
				}

				cout << "One average scalar one-to-"<<target_test_count <<" get_extra_weight_distances_to_targets needs "<< timer / target_test_count <<" musec" << endl;
			}

			{
				long long timer = 0;

				p.reset().pin_targets(test_node_list);
				for(unsigned k=0; k<target_test_count; ++k){
					p
						.reset_source()
						.add_source(test_node_list[k])
						.run_to_pinned_targets();

					timer -= get_micro_time();
					auto scalar_result = p.get_extra_weight_distances_to_targets(extra_scalar_weight, scalar_link);
					timer += get_micro_time();
					
					for(auto x:scalar_result)
						dummy += x;
				}

				cout << "One average scalar one-to-"<<target_test_count <<" get_extra_weight_distances_to_targets with on-demand unpacking needs "<< timer / target_test_count <<" musec" << endl;
			}

			{
				long long timer = 0;

				p.reset().pin_targets(test_node_list);
				for(unsigned k=0; k<target_test_count-3; ++k){
					p
						.reset_source()
						.add_source(test_node_list[k+0])
						.add_source(test_node_list[k+1])
						.add_source(test_node_list[k+2])
						.run_to_pinned_targets();

					timer -= get_micro_time();
					auto scalar_result = p.get_extra_weight_distances_to_targets(ch_extra_scalar_weight, scalar_link);
					timer += get_micro_time();
					
					for(auto x:scalar_result)
						dummy += x;
				}

				cout << "One average scalar three-to-"<<target_test_count <<" get_extra_weight_distances_to_targets needs "<< timer / (target_test_count-3) <<" musec" << endl;
			}


			{
				long long timer = 0;

				p.reset().pin_targets(test_node_list);
				for(unsigned k=0; k<target_test_count; ++k){
					p
						.reset_source()
						.add_source(test_node_list[k])
						.run_to_pinned_targets();

					timer -= get_micro_time();
					auto path_result = p.get_extra_weight_distances_to_targets(ch_extra_path_weight, path_link);
					timer += get_micro_time();
					
					for(auto y:path_result)
						for(auto x:y)
							dummy += x;
				}

				cout << "One average path one-to-"<<target_test_count <<" get_extra_weight_distances_to_targets needs "<< timer / target_test_count <<" musec" << endl;
			}

			{
				long long timer = 0;

				p.reset().pin_targets(test_node_list);
				for(unsigned k=0; k<target_test_count; ++k){
					p
						.reset_source()
						.add_source(test_node_list[k])
						.run_to_pinned_targets();

					timer -= get_micro_time();
					auto path_result = p.get_extra_weight_distances_to_targets(extra_path_weight, path_link);
					timer += get_micro_time();
					
					for(auto y:path_result)
						for(auto x:y)
							dummy += x;
				}

				cout << "One average path one-to-"<<target_test_count <<" get_extra_weight_distances_to_targets with on-demand unpacking needs "<< timer / target_test_count <<" musec" << endl;
			}

			{
				long long timer = 0;

				p.reset().pin_targets(test_node_list);
				for(unsigned k=0; k<target_test_count-3; ++k){
					p
						.reset_source()
						.add_source(test_node_list[k+0])
						.add_source(test_node_list[k+1])
						.add_source(test_node_list[k+2])
						.run_to_pinned_targets();

					timer -= get_micro_time();
					auto path_result = p.get_extra_weight_distances_to_targets(ch_extra_path_weight, path_link);
					timer += get_micro_time();
					
					for(auto y:path_result)
						for(auto x:y)
							dummy += x;
				}

				cout << "One average path three-to-"<<target_test_count <<" get_extra_weight_distances_to_targets needs "<< timer / (target_test_count-3) <<" musec" << endl;
			}

			cout << "dummy output to fool optimizer : "<< dummy << endl; 
		}
	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
		return 1;
	}
	return expect_failed;
}

