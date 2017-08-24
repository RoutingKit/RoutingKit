#include <routingkit/vector_io.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <random>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		unsigned time_count;
		unsigned random_seed;
		unsigned period;
		string source_time_file;
		

		if(argc != 5){
			cerr << argv[0] << " time_count random_seed period_file source_time_file" << endl;
			cerr << "time_count can be a number or a uint32 vector of which the size is taken." << endl;
			return 1;
		}else{
			try{
				time_count = stoul(argv[1]);
			}catch(...){
				time_count = load_vector<unsigned>(argv[1]).size();
			}
			random_seed = stoul(argv[2]);
			period = load_value<unsigned>(argv[3]);
			source_time_file = argv[4];
		}

		cout << "Generating ... " << flush;

		default_random_engine gen(random_seed);
		uniform_int_distribution<int> dist(0, period-1);
		vector<unsigned>source_time(time_count);
		for(auto&x:source_time)
			x = dist(gen);

		cout << "done" << endl;

		cout << "Saving test queries ... " << flush;

		save_vector(source_time_file, source_time);

		cout << "done" << endl;
	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

