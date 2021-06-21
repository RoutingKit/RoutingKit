#include <routingkit/vector_io.h>
#include <routingkit/permutation.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/customizable_contraction_hierarchy.h>
#include <routingkit/min_max.h>
#include <routingkit/timer.h>
#include <routingkit/nested_dissection.h>

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace RoutingKit;
using namespace std;

int test_customizable_contraction_hierarchy(int argc, char*argv[]){

	try{
		long long timer;
	
		cout << "Loading Graph ... " << flush;
		std::vector<unsigned>
			first_out = {0, 0, 1, 2},
			tail = {0, 0, 1, 2},
			head = {1, 2, 3, 3};

		cout << "done" << endl;

		cout << "Loading order ... " << flush;
		
		std::vector<unsigned> cch_order = {1, 2, 3, 4};

		cout << "done" << endl;

		timer = -get_micro_time();
		CustomizableContractionHierarchy cch(cch_order, tail, head, [](const std::string&msg){cout << msg << endl;}, true);
		timer += get_micro_time();

		cout << "Total time spent building CCH is " << timer << "musec" << endl;
		

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
		return 1;
	}
	return 0;
}

