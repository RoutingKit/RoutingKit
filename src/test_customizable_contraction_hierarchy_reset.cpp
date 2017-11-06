#include <routingkit/vector_io.h>
#include <routingkit/permutation.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/customizable_contraction_hierarchy.h>
#include <routingkit/min_max.h>
#include <routingkit/timer.h>

#include "expect.h"

#include <stdexcept>
#include <vector>
#include <random>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{

		const unsigned node_count = 8;

		vector<unsigned>
			tail = {
				0,
				1, 1, 1,
				2,
				3,
				4, 4, 4,
				5,
				6, 6, 6,
				7, 7, 7
			},
			head = {
				4,
				5, 6, 7,
				6,
				7,
				0, 6, 7,
				1,
				1, 2, 4,
				1, 3, 4
			},
			weight1 = {
				2,
				2, 2, 2,
				1,
				1,
				2, 1, 1,
				1,
				1, 1, 1,
				1, 1, 1
			},
			weight2 = {
				3,
				3, 3, 3,
				2,
				2,
				2, 2, 2,
				5,
				15, 4, 3,
				2, 1, 2
			}
		;

		minstd_rand gen;

		vector<unsigned>order1 = random_permutation(node_count, gen);
		vector<unsigned>order2 = random_permutation(node_count, gen);

		CustomizableContractionHierarchy cch1(order1, tail, head);
		CustomizableContractionHierarchy cch2(order2, tail, head);

		{
			CustomizableContractionHierarchyMetric m(cch1, weight1);
			EXPECT_CMP(m.cch, ==, &cch1);
			EXPECT_CMP(m.input_weight, ==, &weight1[0]);
			EXPECT_CMP(m.forward.size(), ==, cch1.cch_arc_count());
			EXPECT_CMP(m.backward.size(), ==, cch1.cch_arc_count());

			m.reset(cch1, weight1);
			EXPECT_CMP(m.cch, ==, &cch1);
			EXPECT_CMP(m.input_weight, ==, &weight1[0]);
			EXPECT_CMP(m.forward.size(), ==, cch1.cch_arc_count());
			EXPECT_CMP(m.backward.size(), ==, cch1.cch_arc_count());

			m.reset(cch1, weight2);
			EXPECT_CMP(m.cch, ==, &cch1);
			EXPECT_CMP(m.input_weight, ==, &weight2[0]);
			EXPECT_CMP(m.forward.size(), ==, cch1.cch_arc_count());
			EXPECT_CMP(m.backward.size(), ==, cch1.cch_arc_count());

			m.reset(cch2, weight2);
			EXPECT_CMP(m.cch, ==, &cch2);
			EXPECT_CMP(m.input_weight, ==, &weight2[0]);
			EXPECT_CMP(m.forward.size(), ==, cch2.cch_arc_count());
			EXPECT_CMP(m.backward.size(), ==, cch2.cch_arc_count());

			m.reset(cch1, weight1);
			EXPECT_CMP(m.cch, ==, &cch1);
			EXPECT_CMP(m.input_weight, ==, &weight1[0]);
			EXPECT_CMP(m.forward.size(), ==, cch1.cch_arc_count());
			EXPECT_CMP(m.backward.size(), ==, cch1.cch_arc_count());
		}

		{
			CustomizableContractionHierarchyMetric m1(cch1, weight1);
			CustomizableContractionHierarchyMetric m2(cch2, weight2);
			CustomizableContractionHierarchyMetric m3(cch1, weight2);

			CustomizableContractionHierarchyQuery q(m1);

			auto is_in_forward_search_space_correct = [&]{
				for(bool x:q.in_forward_search_space)
					if(x)
						return false;
				return true;
			};

			auto is_in_backward_search_space_correct = [&]{
				for(bool x:q.in_backward_search_space)
					if(x)
						return false;
				return true;
			};

			auto is_forward_tentative_distance_correct = [&]{
				for(unsigned x:q.forward_tentative_distance)
					if(x != inf_weight)
						return false;
				return true;
			};

			auto is_backward_tentative_distance_correct = [&]{
				for(unsigned x:q.backward_tentative_distance)
					if(x != inf_weight)
						return false;
				return true;
			};



			EXPECT_CMP(q.metric, ==, &m1);
			EXPECT_CMP(q.cch, ==, &cch1);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());


			q.add_source(0).add_target(4).run().reset();

			EXPECT_CMP(q.metric, ==, &m1);
			EXPECT_CMP(q.cch, ==, &cch1);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

			q.reset(m1);

			EXPECT_CMP(q.metric, ==, &m1);
			EXPECT_CMP(q.cch, ==, &cch1);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

			q.reset(m2);

			EXPECT_CMP(q.metric, ==, &m2);
			EXPECT_CMP(q.cch, ==, &cch2);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

			q.add_source(0).add_source(2).add_target(4).add_target(3).run().reset();

			EXPECT_CMP(q.metric, ==, &m2);
			EXPECT_CMP(q.cch, ==, &cch2);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

			q.pin_targets({0,2,4,6}).add_source(1).run_to_pinned_targets().reset();

			EXPECT_CMP(q.metric, ==, &m2);
			EXPECT_CMP(q.cch, ==, &cch2);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

			q.pin_targets({0,2,4,6}).add_source(1).add_source(3).run_to_pinned_targets().reset();

			EXPECT_CMP(q.metric, ==, &m2);
			EXPECT_CMP(q.cch, ==, &cch2);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

			q.pin_sources({0,2,4,6}).add_target(1).run_to_pinned_sources().reset();

			EXPECT_CMP(q.metric, ==, &m2);
			EXPECT_CMP(q.cch, ==, &cch2);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

			q.pin_sources({0,2,4,6}).add_target(1).add_target(3).run_to_pinned_sources().reset();

			EXPECT_CMP(q.metric, ==, &m2);
			EXPECT_CMP(q.cch, ==, &cch2);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

			q.pin_targets({0,2,4,6}).add_source(1).run_to_pinned_targets().reset(m1);

			EXPECT_CMP(q.metric, ==, &m1);
			EXPECT_CMP(q.cch, ==, &cch1);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

			q.pin_targets({0,2,4,6}).add_source(1).add_source(3).run_to_pinned_targets().reset(m2);

			EXPECT_CMP(q.metric, ==, &m2);
			EXPECT_CMP(q.cch, ==, &cch2);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

			q.pin_sources({0,2,4,6}).add_target(1).run_to_pinned_sources().reset(m1);

			EXPECT_CMP(q.metric, ==, &m1);
			EXPECT_CMP(q.cch, ==, &cch1);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

			q.pin_sources({0,2,4,6}).add_target(1).add_target(3).run_to_pinned_sources().reset(m2);

			EXPECT_CMP(q.metric, ==, &m2);
			EXPECT_CMP(q.cch, ==, &cch2);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());


			q.reset(m1);

			EXPECT_CMP(q.metric, ==, &m1);
			EXPECT_CMP(q.cch, ==, &cch1);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

			q.pin_sources({0,2,4,6}).add_target(1).add_target(3).run_to_pinned_sources().reset(m3);

			EXPECT_CMP(q.metric, ==, &m3);
			EXPECT_CMP(q.cch, ==, &cch1);
			EXPECT(is_in_forward_search_space_correct()); EXPECT(is_in_backward_search_space_correct());
			EXPECT(is_forward_tentative_distance_correct()); EXPECT(is_backward_tentative_distance_correct());
			EXPECT(q.source_node.empty());
			EXPECT(q.source_elimination_tree_end.empty());
			EXPECT(q.target_node.empty());
			EXPECT(q.target_elimination_tree_end.empty());

		}

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
		return 1;
	}
	return expect_failed;
}

