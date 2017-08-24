#include <routingkit/vector_io.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <random>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		unsigned value;
		unsigned size;
		string file;
		

		if(argc != 4){
			cerr << argv[0] << " size value file" << endl;
			return 1;
		}else{
			size = stoul(argv[1]);
			value = stoul(argv[2]);
			file = argv[3];
		}

		cout << "Generating ... " << flush;

		vector<unsigned>v(size, value);

		cout << "done" << endl;

		cout << "Saving test queries ... " << flush;

		save_vector(file, v);

		cout << "done" << endl;
	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

