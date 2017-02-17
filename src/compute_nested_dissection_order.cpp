#include <routingkit/vector_io.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/nested_dissection.h>
#include <routingkit/timer.h>

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		long long timer;
	
		string first_out_file;
		string head_file;
		string latitude_file;
		string longitude_file;
		string cch_order_file;

		if(argc != 6){
			cerr << argv[0] << " first_out head latitude longitude cch_order" << endl;
			return 1;
		}else{
			first_out_file = argv[1];
			head_file = argv[2];
			latitude_file = argv[3];
			longitude_file = argv[4];
			cch_order_file = argv[5];
		}

		cout << "Loading Graph ... " << flush;

		auto first_out = load_vector<unsigned>(first_out_file);
		unsigned node_count = first_out.size()-1;
		auto tail = invert_inverse_vector(first_out);
		auto head = load_vector<unsigned>(head_file);
		auto latitude = load_vector<float>(latitude_file);
		auto longitude = load_vector<float>(longitude_file);

		cout << "done" << endl;

		cout << "Compute order ... " << flush;
		timer = -get_micro_time();
		auto cch_order = compute_nested_node_dissection_order_using_inertial_flow(node_count, tail, head, latitude, longitude);
		timer += get_micro_time();
		cout << "done ["<<timer <<"musec]" << endl;


		cout << "Saving order ... " << flush;
		save_vector(cch_order_file, cch_order);
		cout << "done" << endl;
		
	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

