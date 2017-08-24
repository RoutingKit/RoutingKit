#include <routingkit/vector_io.h>
#include <routingkit/permutation.h>
#include <routingkit/sort.h>
#include <routingkit/timer.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/contraction_hierarchy.h>

#include "verify.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdlib.h>
#include <assert.h>
#include <random>
#include <set>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){
	try{

		ContractionHierarchy ch;

		cerr << "Loading ... " << flush;	
		if(argc != 2){
			cerr
				<< "Usage : \n"
				<< argv[0] << " ch" << endl;
			return 1;
		}else{
			ch = ContractionHierarchy::load_file(argv[1]);
		}
		cerr << "done" << endl;

		cout << "node_count : "<< ch.rank.size() << endl;
		cout << "forward_ch_arc_count : "<< ch.forward.head.size() << endl;
		cout << "backward_ch_arc_count : "<< ch.backward.head.size() << endl;
	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}
