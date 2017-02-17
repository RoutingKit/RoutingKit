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
		string first_out_file;
		string head_file;
		vector<string>weight_file;
		unsigned random_seed;
		
		if(argc < 4){
			cerr << argv[0] << " random_seed first_out_file head_file [weight1_file [weight2_file [...]]]" << endl;
			return 1;
		}else{
			random_seed = stoul(argv[1]);
			first_out_file = argv[2];
			head_file = argv[3];
			for(int i=4; i<argc; ++i)
				weight_file.push_back(argv[i]);
		}

		cout << "Loading graph ... " << flush;

		vector<unsigned>first_out = load_vector<unsigned>(first_out_file);
		vector<unsigned>head = load_vector<unsigned>(head_file);
		vector<vector<unsigned>>weight(weight_file.size());
		for(unsigned i=0; i<weight_file.size(); ++i)
			weight[i] = load_vector<unsigned>(weight_file[i]);

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
		for(unsigned i=0; i<weight.size(); ++i)
			if(weight[i].size() != arc_count)
				throw runtime_error("The "+to_string(i)+"th weight vector must be as long as the number of arcs");

		
		default_random_engine rng(random_seed);

		cout << "Permutating nodes ... " << flush;

		auto node_perm = random_permutation(node_count, rng);
		vector<unsigned>tail = invert_inverse_vector(first_out);

		inplace_apply_permutation_to_elements_of(node_perm, tail);
		inplace_apply_permutation_to_elements_of(node_perm, head);

		{
			auto p = compute_inverse_sort_permutation_using_key(head, node_count, [](unsigned x){return x;});
			tail = apply_inverse_permutation(p, move(tail));
			auto q = compute_inverse_stable_sort_permutation_using_key(tail, node_count, [](unsigned x){return x;});
			tail = apply_inverse_permutation(q, move(tail));
			p = chain_permutation_first_left_then_right(q, p);
			head = apply_inverse_permutation(p, move(head));
			for(unsigned i=0; i<weight.size(); ++i)
				weight[i] = apply_inverse_permutation(p, move(weight[i]));
		}

		first_out = invert_vector(tail, node_count);

		cout << "done" << endl;

		cout << "Saving graph ... " << flush;

		save_vector(first_out_file, first_out);
		save_vector(head_file, head);
		for(unsigned i=0; i<weight_file.size(); ++i)
			save_vector(weight_file[i], weight[i]);

		cout << "done" << endl;

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

