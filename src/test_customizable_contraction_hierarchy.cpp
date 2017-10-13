#include <routingkit/vector_io.h>
#include <routingkit/permutation.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/customizable_contraction_hierarchy.h>
#include <routingkit/min_max.h>
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
		string cch_order_file;

		if(argc != 4){
			cerr << argv[0] << " first_out head cch_order" << endl;
			return 1;
		}else{
			first_out_file = argv[1];
			head_file = argv[2];
			cch_order_file = argv[3];
		}

		cout << "Loading Graph ... " << flush;

		auto first_out = load_vector<unsigned>(first_out_file);
		auto tail = invert_inverse_vector(first_out);
		auto head = load_vector<unsigned>(head_file);

		cout << "done" << endl;

		cout << "Loading order ... " << flush;
		
		auto cch_order = load_vector<unsigned>(cch_order_file);

		cout << "done" << endl;

		timer = -get_micro_time();
		CustomizableContractionHierarchy cch(cch_order, invert_inverse_vector(first_out), head, [](const std::string&msg){cout << msg << endl;}, true);
		timer += get_micro_time();

		cout << "Total time spent building CCH is " << timer << "musec" << endl;
		

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
		return 1;
	}
	return 0;
}

