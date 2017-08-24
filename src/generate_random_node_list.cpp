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
		string file;
		

		if(argc != 5){
			cerr << argv[0] << " node_count query_count random_seed file" << endl;
			cerr << "node_count can be a number or a uint32 vector of which the size is taken." << endl;
			return 1;
		}else{
			try{
				node_count = stoul(argv[1]);
			}catch(...){
				node_count = load_vector<unsigned>(argv[1]).size();
			}
			query_count = stoul(argv[2]);
			random_seed = stoul(argv[3]);
			file = argv[4];
		}

		cout << "Generating ... " << flush;

		default_random_engine gen(random_seed);
		uniform_int_distribution<int> dist(0, node_count-1);
		vector<unsigned>v(query_count);
		for(auto&x:v)
			x = dist(gen);

		cout << "done" << endl;

		cout << "Saving test queries ... " << flush;

		save_vector(file, v);

		cout << "done" << endl;
	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

