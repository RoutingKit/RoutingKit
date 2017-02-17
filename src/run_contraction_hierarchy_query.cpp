#include <routingkit/vector_io.h>
#include <routingkit/permutation.h>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/min_max.h>
#include <routingkit/timer.h>

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		string ch_file;
		string source_file;
		string target_file;
		string distance_file;

		if(argc != 5){
			cerr << argv[0] << " ch_file source_file target_file distance_file" << endl;
			return 1;
		}else{
			ch_file = argv[1];
			source_file = argv[2];
			target_file = argv[3];
			distance_file = argv[4];
		}

		cout << "Loading graph ... " << flush;

		ContractionHierarchy ch = ContractionHierarchy::load_file(ch_file);

		cout << "done" << endl;

		cout << "Loading test queries ... " << flush;

		vector<unsigned>source = load_vector<unsigned>(source_file);
		vector<unsigned>target = load_vector<unsigned>(target_file);

		cout << "done" << endl;

		const unsigned query_count = source.size();

		cout << "Loaded " << query_count << " test queries" << endl;

		vector<unsigned>distance(query_count);
		ContractionHierarchyQuery ch_query(ch);

		cout << "Running test queries ... " << flush;

		long long time_max = 0;
		long long time_sum = 0;

		for(unsigned i=0; i<query_count; ++i){

			long long time = -get_micro_time();
			distance[i] = ch_query.reset().add_source(source[i]).add_target(target[i]).run().get_distance();
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

