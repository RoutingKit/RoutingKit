#include <routingkit/vector_io.h>
#include <routingkit/timer.h>
#include <routingkit/min_max.h>
#include <routingkit/dijkstra.h>
#include <routingkit/inverse_vector.h>

#include "verify.h"

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		string first_out_file;
		string head_file;
		string weight_file;
		string source_file;
		string target_file;
		string distance_file;

		if(argc != 7){
			cerr << argv[0] << " first_out_file head_file weight_file source_file target_file distance_file" << endl;
			return 1;
		}else{
			first_out_file = argv[1];
			head_file = argv[2];
			weight_file = argv[3];
			source_file = argv[4];
			target_file = argv[5];
			distance_file = argv[6];
		}

		cout << "Loading graph ... " << flush;

		vector<unsigned>first_out = load_vector<unsigned>(first_out_file);
		vector<unsigned>head = load_vector<unsigned>(head_file);
		vector<unsigned>weight = load_vector<unsigned>(weight_file);
		
		cout << "done" << endl;

		cout << "Validity tests ... " << flush;
		check_if_graph_is_valid(first_out, head);
		cout << "done" << endl;

		auto tail = invert_inverse_vector(first_out);

		const unsigned node_count = first_out.size()-1;
		const unsigned arc_count = head.size();

		if(first_out.front() != 0)
			throw runtime_error("The first element of first out must be 0.");
		if(first_out.back() != arc_count)
			throw runtime_error("The last element of first out must be the arc count.");
		if(max_element_of(head) >= node_count)
			throw runtime_error("The head vector contains an out-of-bounds node id.");
		if(weight.size() != arc_count)
			throw runtime_error("The weight vector must be as long as the number of arcs");

		Dijkstra dij(first_out, tail, head);

		cout << "Loading test queries ... " << flush;

		vector<unsigned>source = load_vector<unsigned>(source_file);
		vector<unsigned>target = load_vector<unsigned>(target_file);

		cout << "done" << endl;

		if(source.size() != target.size())
			throw runtime_error("The source and target vectors must be of the same length.");
			

		unsigned query_count = source.size();

		cout << "Loaded " << query_count << " test queries" << endl;

		vector<unsigned>distance(query_count);

		cout << "Running test queries ... " << flush;

		long long time_max = 0;
		long long time_sum = 0;

		for(unsigned i=0; i<query_count; ++i){

			long long time = -get_micro_time();

			dij.reset().add_source(source[i]);
			while(!dij.is_finished()){
				auto x = dij.settle(ScalarGetWeight(weight)).node;
				if(x == target[i])
					break;
			}
			distance[i] = dij.get_distance_to(target[i]);

			time += get_micro_time();

			time_max = std::max(time_max, time);
			time_sum += time;
		}

		cout << "done" << endl;

		cout << "max running time : " << time_max << "musec" << endl;
		cout << "avg running time : " << time_sum/query_count << "musec" << endl;

		save_vector(distance_file, distance);

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

