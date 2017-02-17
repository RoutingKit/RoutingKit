#include <routingkit/vector_io.h>
#include <routingkit/timer.h>
#include <routingkit/contraction_hierarchy.h>

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		string first_out_file;
		string head_file;
		string weight_file;
		string dot_file;
		
		if(argc != 5){
			cerr << argv[0] << " first_out_file head_file weight_file dot_file" << endl;
			return 1;
		}else{
			first_out_file = argv[1];
			head_file = argv[2];
			weight_file = argv[3];
			dot_file = argv[4];
		}

		cout << "Loading graph ... " << flush;

		vector<unsigned>first_out_arc = load_vector<unsigned>(first_out_file);
		vector<unsigned>head = load_vector<unsigned>(head_file);
		vector<unsigned>weight = load_vector<unsigned>(weight_file);

		cout << "done" << endl;

		cout << "Writing dot ... " << flush;

		std::ofstream out(dot_file);
		if(!out)
			throw std::runtime_error("Can not open file "+dot_file);
		out << "digraph G{";
		for(unsigned x=0; x<first_out_arc.size()-1; ++x){
			for(unsigned xy=first_out_arc[x]; xy<first_out_arc[x+1]; ++xy){
				unsigned y=head[xy];
				unsigned w=weight[xy];
				out << x << " -> " << y << "[label=\"" << w << "\"];\n";
			}
		}
		out << "}\n";

		cout << "done" << endl;

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

