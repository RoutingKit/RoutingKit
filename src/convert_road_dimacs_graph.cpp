#include <routingkit/vector_io.h>
#include <routingkit/permutation.h>
#include <routingkit/sort.h>
#include <routingkit/inverse_vector.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		string dimacs_file;
		string first_out_file;
		string head_file;
		string weight_file;

		if(argc != 5){
			cerr << argv[0] << " dimacs_file first_out_file head_file weight_file" << endl;
			return 1;
		}else{
			dimacs_file = argv[1];
			first_out_file = argv[2];
			head_file = argv[3];
			weight_file = argv[4];
		}

		cout << "Loading data ... " << flush;

		ifstream in(dimacs_file);
		if(!in)
			throw runtime_error("Can not open \""+dimacs_file+"\"");

		string line;
		unsigned line_num = 0;
		unsigned next_arc = 0;

		vector<unsigned>tail, head, weight;
		unsigned node_count, arc_count;

		bool was_header_read = false;
		while(std::getline(in, line)){
			++line_num;
			if(line.empty() || line[0] == 'c')
				continue;

			std::istringstream lin(line);
			if(!was_header_read){
				was_header_read = true;
				std::string p, sp;
				if(!(lin >> p >> sp >> node_count >> arc_count))
					throw std::runtime_error("Can not parse header in dimacs file.");
				if(p != "p" || sp != "sp")
					throw std::runtime_error("Invalid header in dimacs file.");

				tail.resize(arc_count);
				head.resize(arc_count);
				weight.resize(arc_count);
			}else{
				std::string a;
				unsigned h, t, w;
				if(!(lin >> a >> t >> h >> w))
					throw std::runtime_error("Can not parse line num "+std::to_string(line_num)+" \""+line+"\" in dimacs file.");
				--h;
				--t;
				if(a != "a" || h >= node_count || t >= node_count)
					throw std::runtime_error("Invalid arc in line num "+std::to_string(line_num)+" \""+line+"\" in dimacs file.");
				if(next_arc < arc_count){
					head[next_arc] = h;
					tail[next_arc] = t;
					weight[next_arc] = w;
				}
				++next_arc;
			}
		}

		if(next_arc != arc_count)
			throw std::runtime_error("The arc count in the header ("+to_string(arc_count)+") does not correspond with the actual number of arcs ("+to_string(next_arc)+").");

		cout << "done" << endl;

		cout << "Ordering arcs ... " << flush;

		vector<unsigned>first_out;

		{
			auto p = compute_inverse_stable_sort_permutation_using_key(head, node_count, [](unsigned x){return x;});
			tail = apply_inverse_permutation(p, tail);
			auto q = compute_inverse_stable_sort_permutation_using_key(tail, node_count, [](unsigned x){return x;});
			tail = apply_inverse_permutation(q, tail);
			auto r = chain_permutation_first_left_then_right(q, p);
			head = apply_inverse_permutation(r, head);
			weight = apply_inverse_permutation(r, weight);
			first_out = invert_vector(tail, node_count);
		}

		cout << "done" << endl;

		cout << "Saving file ... " << flush;
		save_vector(first_out_file, first_out);
		save_vector(head_file, head);
		save_vector(weight_file, weight);
		cout << "done" << endl;

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}
