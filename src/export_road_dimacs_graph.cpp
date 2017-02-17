#include <routingkit/vector_io.h>
#include <routingkit/permutation.h>
#include <routingkit/sort.h>
#include <routingkit/inverse_vector.h>

#include <iostream>
#include <stdexcept>
#include <random>
#include <vector>
#include <string>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		string in_first_out_file;
		string in_head_file;
		string in_weight_file;

		string out_dimacs_file;
		
		if(argc != 5){
			cerr << argv[0] << " in_first_out in_head in_weight out.gr" << endl;
			return 1;
		} else {
			in_first_out_file = argv[1];
			in_head_file = argv[2];
			in_weight_file = argv[3];

			out_dimacs_file = argv[4];
		}

		cout << "Loading graph ... " << flush;

		vector<unsigned>first_out = load_vector<unsigned>(in_first_out_file);
		vector<unsigned>head = load_vector<unsigned>(in_head_file);
		vector<unsigned>weight = load_vector<unsigned>(in_weight_file);
		vector<unsigned>tail = invert_inverse_vector(first_out);
		
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


		cout << "Saving graph ... " << flush;

		open_file_for_saving(
			out_dimacs_file,
			[&](std::ostream&out){
				out << "p sp "<< node_count << " " << arc_count << '\n';
				for(unsigned i=0; i<arc_count; ++i){
					out << "a " << (tail[i]+1) << " " << (head[i]+1) << " " << weight[i] << '\n';
				}
			}
		);

		cout << "done" << endl;

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

