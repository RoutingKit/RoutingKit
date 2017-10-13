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
		string weight_file;
		string cch_order_file;

		if(argc != 5){
			cerr << argv[0] << " first_out head weight_file cch_order" << endl;
			return 1;
		}else{
			first_out_file = argv[1];
			head_file = argv[2];
			weight_file = argv[3];
			cch_order_file = argv[4];
		}

		cout << "Loading Graph ... " << flush;

		auto first_out = load_vector<unsigned>(first_out_file);
		auto tail = invert_inverse_vector(first_out);
		auto head = load_vector<unsigned>(head_file);
		auto weight = load_vector<unsigned>(weight_file);

		cout << "done" << endl;

		cout << "Loading order ... " << flush;
		
		auto cch_order = load_vector<unsigned>(cch_order_file);

		cout << "done" << endl;

		cout << "Building CCH ... " << flush;
		
		timer = -get_micro_time();
		CustomizableContractionHierarchy cch(cch_order, invert_inverse_vector(first_out), head);
		timer += get_micro_time();

		cout << "done [" << timer << "musec]" << endl;

		cout << "Customizing CCH ... " << flush;
		
		timer = -get_micro_time();
		CustomizableContractionHierarchyMetric metric(cch, weight);
		metric.customize();
		timer += get_micro_time();

		cout << "done [" << timer << "musec]" << endl;

		cout << "Generating queries ... " << flush;

		const unsigned
			source_count = 500,
			target_count = 500;

		std::vector<unsigned>
			source_set(source_count),
			target_set(target_count);

		for(unsigned i=0; i<source_count; ++i)
			source_set[i] = rand() % cch.node_count();
		for(unsigned i=0; i<target_count; ++i)
			target_set[i] = rand() % cch.node_count();
		
		cout << "done" << endl;


		std::vector<unsigned>optimal_result(source_count * target_count);

		{
			timer = -get_micro_time();
			cout << "Running baseline ... " << flush;
		
			CustomizableContractionHierarchyQuery query(metric);
			for(unsigned s=0; s<source_count; ++s){
				for(unsigned t=0; t<target_count; ++t){
					optimal_result[s*target_count + t] = query.reset().add_source(source_set[s]).add_target(target_set[t]).run().get_distance();
				}
			}

			timer += get_micro_time();

			cout << "done ["<<timer << "musec]" << endl;
		}


		std::vector<unsigned>pinned_source_result(source_count * target_count);

		{


			long long select_time = 0;
			unsigned select_count = 0;
			long long query_time = 0;
			unsigned query_count = 0;

			timer = -get_micro_time();
			cout << "Running forward pinning ... " << flush;
		
			CustomizableContractionHierarchyQuery query(metric);

			select_time -= get_micro_time();
			query.reset();
			query.pin_targets(target_set);
			select_time += get_micro_time();
			++select_count;

			for(unsigned s=0; s<source_count; ++s){

				query_time -= get_micro_time();
				auto d = query.reset_source().add_source(source_set[s]).run_to_pinned_targets().get_distances_to_targets();

				for(unsigned t=0; t<target_count; ++t){
					pinned_source_result[s*target_count + t] = d[t];
				}
				query_time += get_micro_time();
				++query_count;
			}

			timer += get_micro_time();

			cout << "done [" << timer << "musec]" << endl;

			cout << "query_time" << " : " << query_time / query_count << "musec" << endl;
			cout << "pin_time" << " : " << select_time / select_count << "musec" << endl;
		}

		if(pinned_source_result != optimal_result){
			cout << "Pinned source is not correct" << endl;
		} else {
			cout << "No error with pinned source found" << endl;
		}

		std::vector<unsigned>pinned_target_result(source_count * target_count);

		{


			long long select_time = 0;
			unsigned select_count = 0;
			long long query_time = 0;
			unsigned query_count = 0;

			timer = -get_micro_time();
			cout << "Running forward pinning ... " << flush;
		
			CustomizableContractionHierarchyQuery query(metric);

			select_time -= get_micro_time();
			query.reset();
			query.pin_sources(source_set);
			select_time += get_micro_time();
			++select_count;

			for(unsigned t=0; t<target_count; ++t){

				query_time -= get_micro_time();
				auto d = query.reset_target().add_target(target_set[t]).run_to_pinned_sources().get_distances_to_sources();
				for(unsigned s=0; s<source_count; ++s){
					pinned_target_result[s*target_count + t] = d[s];
				}
				query_time += get_micro_time();
				++query_count;
			}

			timer += get_micro_time();

			cout << "done [" << timer << "musec]" << endl;

			cout << "query_time" << " : " << query_time / query_count << "musec" << endl;
			cout << "pin_time" << " : " << select_time / select_count << "musec" << endl;

		}

		if(pinned_target_result != optimal_result){
			cout << "Pinned target is not correct" << endl;
		} else {
			cout << "No error with pinned target found" << endl;
		}

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
		return 1;
	}
	return 0;
}

