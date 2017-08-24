#include <routingkit/vector_io.h>
#include <routingkit/timer.h>
#include <routingkit/min_max.h>
#include <routingkit/geo_dist.h>

#include "verify.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <math.h>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		string first_out_file;
		string head_file;
		string weight_file;
		string latitude_file;
		string longitude_file;

		if(argc != 6){
			cerr << argv[0] << " first_out_file head_file latitude_file longitude_file weight_file " << endl;
			return 1;
		}else{
			first_out_file = argv[1];
			head_file = argv[2];
			latitude_file = argv[3];
			longitude_file = argv[4];
			weight_file = argv[5];
		}

		cout << "Loading graph ... " << flush;

		vector<unsigned>first_out = load_vector<unsigned>(first_out_file);
		vector<unsigned>head = load_vector<unsigned>(head_file);

		vector<float>latitude = load_vector<float>(latitude_file);
		vector<float>longitude = load_vector<float>(longitude_file);

		cout << "done" << endl;

		cout << "Validity tests ... " << flush;
		check_if_graph_is_valid(first_out, head);
		cout << "done" << endl;


		cout << "Computing weights ... " << flush;
		
		std::vector<unsigned>weight(head.size());

		for(unsigned x=0; x<first_out.size()-1; ++x){
			for(unsigned xy=first_out[x]; xy<first_out[x+1]; ++xy){
				unsigned y=head[xy];
				weight[xy] = static_cast<unsigned>(geo_dist(latitude[x], longitude[x], latitude[y], longitude[y]));
			}
		}

		cout << "done" << endl;

		cout << "Saving weights ... " << flush;
		
		save_vector(weight_file, weight);

		cout << "done" << endl;

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

