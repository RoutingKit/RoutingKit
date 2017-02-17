#include <routingkit/vector_io.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <random>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		unsigned node_count;
		unsigned query_count;
		unsigned random_seed;
		string source_file;
		string target_file;

		if(argc != 6){
			cerr << argv[0] << " node_count query_count random_seed source_file target_file" << endl;
			return 1;
		}else{
			node_count = stoul(argv[1]);
			query_count = stoul(argv[2]);
			random_seed = stoul(argv[3]);
			source_file = argv[4];
			target_file = argv[5];
		}

		vector<unsigned>source(query_count);
		vector<unsigned>target(query_count);
		
		cout << "Generating test queries ... " << flush;

		std::default_random_engine gen(random_seed);
		std::uniform_int_distribution<int> dist(0, node_count-1);

		for(unsigned i=0; i<query_count; ++i){
			source[i] = dist(gen);
			target[i] = dist(gen);
		}

		cout << "done" << endl;

		cout << "Saving test queries ... " << flush;

		save_vector(source_file, source);
		save_vector(target_file, target);

		cout << "done" << endl;
	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

