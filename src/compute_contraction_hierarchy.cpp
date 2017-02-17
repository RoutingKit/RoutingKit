#include <routingkit/vector_io.h>
#include <routingkit/timer.h>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/min_max.h>
#include <routingkit/inverse_vector.h>

#include "verify.h"

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		string graph_first_out;
		string graph_head;
		string graph_weight;

		string ch_file;

		if(argc != 5){
			cerr << argv[0] << " graph_first_out graph_head graph_weight ch_file" << endl;
			return 1;
		}else{
			graph_first_out = argv[1];
			graph_head = argv[2];
			graph_weight = argv[3];
			ch_file = argv[4];
		}

		cout << "Loading graph ... " << flush;

		vector<unsigned>first_out = load_vector<unsigned>(graph_first_out);
		vector<unsigned>head = load_vector<unsigned>(graph_head);
		vector<unsigned>weight = load_vector<unsigned>(graph_weight);

		cout << "done" << endl;

		cout << "Validity tests ... " << flush;
		check_if_graph_is_valid(first_out, head);
		cout << "done" << endl;


		const unsigned node_count = first_out.size()-1;
		const unsigned arc_count = head.size();

		if(first_out.front() != 0)
			throw runtime_error("The first element of first out must be 0.");
		if(first_out.back() != arc_count)
			throw runtime_error("The last element of first out must be the arc count.");
		if(head.empty())
			throw runtime_error("The head vector must not be empty.");
		if(max_element_of(head) >= node_count)
			throw runtime_error("The head vector contains an out-of-bounds node id.");
		if(weight.size() != arc_count)
			throw runtime_error("The weight vector must be as long as the number of arcs");

		
		auto ch = ContractionHierarchy::build(node_count, invert_inverse_vector(first_out), head, weight, [](string msg){cout << msg << endl;});
		check_contraction_hierarchy_for_errors(ch);
		ch.save_file(ch_file);

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

