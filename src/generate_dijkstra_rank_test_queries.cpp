#include <routingkit/vector_io.h>
#include <routingkit/dijkstra.h>
#include <routingkit/inverse_vector.h>

#include "verify.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <random>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		unsigned source_node_count;
		unsigned random_seed;
		string source_file;
		string target_file;
		string rank_file;
		string distance_file;

		vector<unsigned>first_out;	
		vector<unsigned>head;
		vector<unsigned>weight;

		if(argc != 10){
			cerr << argv[0] << " first_out head weight source_node_count random_seed source_file target_file rank_file distance_file" << endl;
			cerr << "source_node_count can be a number or a uint32 vector of which the size is taken." << endl;
			return 1;
		}else{
			first_out = load_vector<unsigned>(argv[1]);
			head = load_vector<unsigned>(argv[2]);
			weight = load_vector<unsigned>(argv[3]);

			try{
				source_node_count = stoul(argv[4]);
			}catch(...){
				source_node_count = load_vector<unsigned>(argv[4]).size();
			}
			random_seed = stoul(argv[5]);
			source_file = argv[6];
			target_file = argv[7];
			rank_file = argv[8];
			distance_file = argv[9];
		}

		cout << "Validity tests ... " << flush;
		check_if_graph_is_valid(first_out, head);
		cout << "done" << endl;

		auto tail = invert_inverse_vector(first_out);

		unsigned node_count = first_out.size()-1;

		vector<unsigned>source;
		vector<unsigned>target;
		vector<unsigned>rank;
		vector<unsigned>distance;

		
		cout << "Generating test queries ... " << flush;

		std::default_random_engine gen(random_seed);
		std::uniform_int_distribution<int> dist(0, node_count-1);

		Dijkstra dij(first_out, tail, head);

		for(unsigned i=0; i<source_node_count; ++i){
			
			unsigned source_node = dist(gen);
			unsigned r = 0;
			unsigned n = 0;

			dij.reset().add_source(source_node);
			
			while(!dij.is_finished()){
				auto x = dij.settle(ScalarGetWeight(weight));
				++n;
				if(n == (1u << r)){

					if(r > 5){
						source.push_back(source_node);
						target.push_back(x.node);
						rank.push_back(r);
						distance.push_back(x.distance);
					}
					
					++r;
				}
			}
		}

		cout << "done" << endl;

		cout << "Saving test queries ... " << flush;

		save_vector(source_file, source);
		save_vector(target_file, target);
		save_vector(rank_file, rank);
		save_vector(distance_file, distance);

		cout << "done" << endl;
	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

