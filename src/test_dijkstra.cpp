#include <routingkit/bit_vector.h>
#include <routingkit/vector_io.h>
#include <routingkit/dijkstra.h>
#include <routingkit/inverse_vector.h>

#include <vector>
#include <random>

#include "expect.h"
#include "verify.h"

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){
	try{
		if(argc != 4){
			cout << argv[0] << " first_out head weight" << endl;
			return 1;
		}
		cout << "Loading data ... " << flush;
		vector<unsigned>
			first_out = load_vector<unsigned>(argv[1]),
			head = load_vector<unsigned>(argv[2]),
			weight = load_vector<unsigned>(argv[3]);
		cout << "done" << endl;

		// add 2 isolated nodes
		first_out.push_back(first_out.back()+1);
		first_out.push_back(first_out.back()+1);
		unsigned isolated_node_count = 2;
		unsigned isolated_node1 = first_out.size()-3;
		unsigned isolated_node2 = first_out.size()-2;
		head.push_back(isolated_node2);
		weight.push_back(0);
		head.push_back(isolated_node1);
		weight.push_back(42);

		cout << "Validity tests ... " << flush;
		check_if_graph_is_valid(first_out, head);
		cout << "done" << endl;

		vector<unsigned>tail = invert_inverse_vector(first_out);

		const unsigned path_test_count = 1000;
		const unsigned test_count = 1000;
		unsigned node_count = first_out.size()-1;
		unsigned arc_count = head.size();


		minstd_rand gen;
		uniform_int_distribution<unsigned>node_dist(0, node_count-isolated_node_count-1);

		Dijkstra dij(first_out, tail, head);
		EXPECT(dij.is_finished());
		for(unsigned test_num=0; test_num < test_count; ++test_num){
			vector<unsigned>dist(node_count, inf_weight);

			unsigned source_node = node_dist(gen);
			unsigned source_time = test_num*100;
			dij.reset().add_source(source_node, source_time);

			while(!dij.is_finished()){
				auto ret = dij.settle(ScalarGetWeight(weight));
				EXPECT_CMP(ret.node, !=, isolated_node1);
				EXPECT_CMP(ret.node, !=, isolated_node2);
				EXPECT_CMP(ret.node, <, node_count);
				EXPECT_CMP(ret.distance, <, inf_weight);
				EXPECT_CMP(dist[ret.node], ==, inf_weight);
				dist[ret.node] = ret.distance;
			}

			EXPECT_CMP(dij.get_distance_to(isolated_node1), ==, inf_weight);
			EXPECT_CMP(dij.get_distance_to(isolated_node2), ==, inf_weight);

			for(unsigned x=0; x<node_count; ++x)
				EXPECT_CMP(dist[x], ==, dij.get_distance_to(x));

			{
				BitVector witness_found(node_count, false);
				for(unsigned xy=0; xy<arc_count; ++xy){
					unsigned x=tail[xy], y=head[xy];
					EXPECT_CMP(dist[x] + weight[xy], >=, dist[y]);
					if(dist[x] + weight[xy] == dist[y] && dist[y] != inf_weight)
						witness_found.set(y);
				}

				for(unsigned x=0; x<node_count; ++x)
					EXPECT(witness_found.is_set(x) || dist[x] == inf_weight || x == source_node);
				EXPECT(!witness_found.is_set(isolated_node1));
				EXPECT(!witness_found.is_set(isolated_node2));
			}


			for(unsigned path_test_num=0; path_test_num < path_test_count; ++path_test_num){
				unsigned target_node = node_dist(gen);
				if(path_test_num == path_test_count/2)
					target_node = source_node;

				auto arc_path = dij.get_arc_path_to(target_node);
				auto node_path = dij.get_node_path_to(target_node);

				for(auto a:arc_path)
					EXPECT_CMP(a, <=, arc_count);
				for(auto x:node_path)
					EXPECT_CMP(x, <=, node_count);


				if(dij.get_distance_to(target_node) == inf_weight){
					EXPECT(node_path.empty());
					EXPECT(arc_path.empty());
				}else{
					EXPECT_CMP(node_path.size(), ==, arc_path.size()+1);

					if(arc_path.empty()){
						EXPECT_CMP(source_node, ==, target_node);
						EXPECT_CMP(dij.get_distance_to(target_node), ==, source_time);
					}else{
						for(unsigned i=0; i<arc_path.size(); ++i)
							EXPECT_CMP(tail[arc_path[i]], ==, node_path[i]);
						EXPECT_CMP(head[arc_path.back()], ==, node_path.back());

						EXPECT_CMP(node_path.front(), ==, source_node);
						EXPECT_CMP(node_path.back(), ==, target_node);
							
						unsigned actual_length = source_time;
						for(auto a:arc_path){
							actual_length += weight[a];
						}
						EXPECT_CMP(actual_length, ==, dij.get_distance_to(target_node));
					}
				}
			}
		}

		{
			dij.reset(first_out, tail, head).add_source(isolated_node1);
			EXPECT(!dij.is_finished());
			auto r1 = dij.settle(ScalarGetWeight(weight));
			EXPECT_CMP(r1.node, ==, isolated_node1);
			EXPECT_CMP(r1.distance, ==, 0);
			
			EXPECT(!dij.is_finished());
			auto r2 = dij.settle(ScalarGetWeight(weight));
			EXPECT_CMP(r2.node, ==, isolated_node2);
			EXPECT_CMP(r2.distance, ==, 0);
			
			EXPECT(dij.is_finished());
		}

		{
			dij.reset().add_source(isolated_node2);
			EXPECT(!dij.is_finished());
			auto r1 = dij.settle(ScalarGetWeight(weight));
			EXPECT_CMP(r1.node, ==, isolated_node2);
			EXPECT_CMP(r1.distance, ==, 0);
			
			EXPECT(!dij.is_finished());
			auto r2 = dij.settle(ScalarGetWeight(weight));
			EXPECT_CMP(r2.node, ==, isolated_node1);
			EXPECT_CMP(r2.distance, ==, 42);
			
			EXPECT(dij.is_finished());
		}
	}catch(exception&err){
		cout << "Stopped on exception : "<< err.what() << endl;
		return 1;
	}
	return expect_failed;
}
