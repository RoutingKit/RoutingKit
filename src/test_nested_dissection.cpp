#include <routingkit/nested_dissection.h>
#include <routingkit/inverse_vector.h>

#include "expect.h"

#include <vector>

using namespace RoutingKit;
using namespace std;

int main(){


	{
		unsigned node_count = 4;
		std::vector<unsigned>
			tail = { 0, 0, 1, 3},
			head = { 1, 2, 3, 2};

		auto g = make_graph_fragment(node_count, tail, head);

		EXPECT_CMP(g.tail.size(), ==, 8);
		EXPECT_CMP(g.head.size(), ==, 8);
		EXPECT_CMP(g.back_arc.size(), ==, 8);
		EXPECT_CMP(g.global_node_id.size(), ==, 4);
		EXPECT(is_permutation(g.global_node_id));

		EXPECT_CMP(g.arc_count(), ==, 8);
		EXPECT_CMP(g.node_count(), ==, 4);

		EXPECT_CMP(g.tail[0], ==, 0);
		EXPECT_CMP(g.tail[1], ==, 0);
		EXPECT_CMP(g.tail[2], ==, 1);
		EXPECT_CMP(g.tail[3], ==, 1);
		EXPECT_CMP(g.tail[4], ==, 2);
		EXPECT_CMP(g.tail[5], ==, 2);
		EXPECT_CMP(g.tail[6], ==, 3);
		EXPECT_CMP(g.tail[7], ==, 3);

		EXPECT_CMP(g.head[0], ==, 1);
		EXPECT_CMP(g.head[1], ==, 2);
		EXPECT_CMP(g.head[2], ==, 0);
		EXPECT_CMP(g.head[3], ==, 3);
		EXPECT_CMP(g.head[4], ==, 0);
		EXPECT_CMP(g.head[5], ==, 3);
		EXPECT_CMP(g.head[6], ==, 1);
		EXPECT_CMP(g.head[7], ==, 2);

		EXPECT_CMP(g.back_arc[0], ==, 2);
		EXPECT_CMP(g.back_arc[1], ==, 4);
		EXPECT_CMP(g.back_arc[2], ==, 0);
		EXPECT_CMP(g.back_arc[3], ==, 6);
		EXPECT_CMP(g.back_arc[4], ==, 1);
		EXPECT_CMP(g.back_arc[5], ==, 7);
		EXPECT_CMP(g.back_arc[6], ==, 3);
		EXPECT_CMP(g.back_arc[7], ==, 5);

	}


	{
		unsigned node_count = 4;
		std::vector<unsigned>
			tail = { 0, 0, 1, 2},
			head = { 0, 0, 2, 2};

		auto g = make_graph_fragment(node_count, tail, head);

		EXPECT_CMP(g.tail.size(), ==, 2);
		EXPECT_CMP(g.head.size(), ==, 2);
		EXPECT_CMP(g.back_arc.size(), ==, 2);

		EXPECT_CMP(g.tail[0], ==, 1);
		EXPECT_CMP(g.tail[1], ==, 2);

		EXPECT_CMP(g.head[0], ==, 2);
		EXPECT_CMP(g.head[1], ==, 1);

		EXPECT_CMP(g.back_arc[0], ==, 1);
		EXPECT_CMP(g.back_arc[1], ==, 0);
	}


	{
		unsigned node_count = 15;
		std::vector<unsigned>
			tail = { 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 5, 5, 5, 6, 6, 7, 7, 7, 8, 8,10},
			head = { 1, 2, 2, 4,13, 3, 4, 5,12, 6, 7, 9,10, 7,11, 8,12,14,12,13,11},
			source_list = {0, 2},
			target_list = {7, 14};
	
		auto g = make_graph_fragment(node_count, tail, head);

		BitVector is_source(node_count, false);
		is_source.set(0);
		is_source.set(2);

		BitVector is_target(node_count, false);
		is_target.set(7);
		is_target.set(14);

		BlockingFlow algo(g, is_source, is_target);

		while(!algo.is_finished())
			algo.advance();

		{
			auto c = algo.get_source_cut();

			EXPECT_CMP(c.cut_size, ==, 3);
			EXPECT_CMP(c.node_on_side_count, ==, 4);
		
			EXPECT(c.is_node_on_side.is_set(0));
			EXPECT(c.is_node_on_side.is_set(1));
			EXPECT(c.is_node_on_side.is_set(2));
			EXPECT(!c.is_node_on_side.is_set(3));
			EXPECT(c.is_node_on_side.is_set(4));
			EXPECT(!c.is_node_on_side.is_set(5));
			EXPECT(!c.is_node_on_side.is_set(6));
			EXPECT(!c.is_node_on_side.is_set(7));
			EXPECT(!c.is_node_on_side.is_set(8));
			EXPECT(!c.is_node_on_side.is_set(9));
			EXPECT(!c.is_node_on_side.is_set(10));
			EXPECT(!c.is_node_on_side.is_set(11));
			EXPECT(!c.is_node_on_side.is_set(12));
			EXPECT(!c.is_node_on_side.is_set(13));
			EXPECT(!c.is_node_on_side.is_set(14));
		}

		{
			auto c = algo.get_target_cut();

			EXPECT_CMP(c.cut_size, ==, 3);
			EXPECT_CMP(c.node_on_side_count, ==, 7);
		
			EXPECT(!c.is_node_on_side.is_set(0));
			EXPECT(!c.is_node_on_side.is_set(1));
			EXPECT(!c.is_node_on_side.is_set(2));
			EXPECT(!c.is_node_on_side.is_set(3));
			EXPECT(!c.is_node_on_side.is_set(4));
			EXPECT(c.is_node_on_side.is_set(5));
			EXPECT(c.is_node_on_side.is_set(6));
			EXPECT(c.is_node_on_side.is_set(7));
			EXPECT(!c.is_node_on_side.is_set(8));
			EXPECT(c.is_node_on_side.is_set(9));
			EXPECT(c.is_node_on_side.is_set(10));
			EXPECT(c.is_node_on_side.is_set(11));
			EXPECT(!c.is_node_on_side.is_set(12));
			EXPECT(!c.is_node_on_side.is_set(13));
			EXPECT(c.is_node_on_side.is_set(14));
		}


	}

	{
		unsigned node_count = 15;
		std::vector<unsigned>
			tail = { 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 7, 7, 7, 7, 7, 8, 8, 8, 9,10,10,11,11,12,12,12,13,13,14},
			head = { 1, 2, 0, 2, 4,13, 0, 1, 3, 4, 2, 5, 1, 2,12, 3, 6, 7, 9,10, 5, 7,11, 5, 6, 8,12,14, 7,12,13, 5, 5,11, 6,10, 4, 7, 8, 1, 8, 7};
	
		auto g = make_graph_fragment(node_count, tail, head);

		BitVector is_source(node_count, false);
		is_source.set(0);
		is_source.set(2);

		BitVector is_target(node_count, false);
		is_target.set(7);
		is_target.set(14);

		BlockingFlow algo(g, is_source, is_target);

		while(!algo.is_finished())
			algo.advance();

		{
			auto c = algo.get_source_cut();

			EXPECT_CMP(c.cut_size, ==, 6);
			EXPECT_CMP(c.node_on_side_count, ==, 4);
		
			EXPECT(c.is_node_on_side.is_set(0));
			EXPECT(c.is_node_on_side.is_set(1));
			EXPECT(c.is_node_on_side.is_set(2));
			EXPECT(!c.is_node_on_side.is_set(3));
			EXPECT(c.is_node_on_side.is_set(4));
			EXPECT(!c.is_node_on_side.is_set(5));
			EXPECT(!c.is_node_on_side.is_set(6));
			EXPECT(!c.is_node_on_side.is_set(7));
			EXPECT(!c.is_node_on_side.is_set(8));
			EXPECT(!c.is_node_on_side.is_set(9));
			EXPECT(!c.is_node_on_side.is_set(10));
			EXPECT(!c.is_node_on_side.is_set(11));
			EXPECT(!c.is_node_on_side.is_set(12));
			EXPECT(!c.is_node_on_side.is_set(13));
			EXPECT(!c.is_node_on_side.is_set(14));
		}

		{
			auto c = algo.get_target_cut();

			EXPECT_CMP(c.cut_size, ==, 6);
			EXPECT_CMP(c.node_on_side_count, ==, 7);
		
			EXPECT(!c.is_node_on_side.is_set(0));
			EXPECT(!c.is_node_on_side.is_set(1));
			EXPECT(!c.is_node_on_side.is_set(2));
			EXPECT(!c.is_node_on_side.is_set(3));
			EXPECT(!c.is_node_on_side.is_set(4));
			EXPECT(c.is_node_on_side.is_set(5));
			EXPECT(c.is_node_on_side.is_set(6));
			EXPECT(c.is_node_on_side.is_set(7));
			EXPECT(!c.is_node_on_side.is_set(8));
			EXPECT(c.is_node_on_side.is_set(9));
			EXPECT(c.is_node_on_side.is_set(10));
			EXPECT(c.is_node_on_side.is_set(11));
			EXPECT(!c.is_node_on_side.is_set(12));
			EXPECT(!c.is_node_on_side.is_set(13));
			EXPECT(c.is_node_on_side.is_set(14));
		}


	}


	{
		unsigned node_count = 8;
		std::vector<unsigned>
			tail = {0,0,1,1,2,4,5,6,7},
			head = {1,4,2,6,3,5,2,7,3};
	
		auto g = make_graph_fragment(node_count, tail, head);

		BitVector is_source(node_count, false);
		is_source.set(0);

		BitVector is_target(node_count, false);
		is_target.set(3);

		BlockingFlow algo(g, is_source, is_target);

		while(!algo.is_finished())
			algo.advance();

		EXPECT_CMP(algo.get_current_flow_intensity(), ==, 2);
			

		{
			auto c = algo.get_source_cut();

			EXPECT_CMP(c.cut_size, ==, 2);
			EXPECT_CMP(c.node_on_side_count, ==, 1);
		
			EXPECT(c.is_node_on_side.is_set(0));
			EXPECT(!c.is_node_on_side.is_set(1));
			EXPECT(!c.is_node_on_side.is_set(2));
			EXPECT(!c.is_node_on_side.is_set(3));
			EXPECT(!c.is_node_on_side.is_set(4));
			EXPECT(!c.is_node_on_side.is_set(5));
			EXPECT(!c.is_node_on_side.is_set(6));
			EXPECT(!c.is_node_on_side.is_set(7));
		}

		{
			auto c = algo.get_target_cut();

			EXPECT_CMP(c.cut_size, ==, 2);
			EXPECT_CMP(c.node_on_side_count, ==, 1);
		
			EXPECT(!c.is_node_on_side.is_set(0));
			EXPECT(!c.is_node_on_side.is_set(1));
			EXPECT(!c.is_node_on_side.is_set(2));
			EXPECT(c.is_node_on_side.is_set(3));
			EXPECT(!c.is_node_on_side.is_set(4));
			EXPECT(!c.is_node_on_side.is_set(5));
			EXPECT(!c.is_node_on_side.is_set(6));
			EXPECT(!c.is_node_on_side.is_set(7));
		}
	}


	{
		unsigned node_count = 10;
		std::vector<unsigned>
			tail = {0,1,2,3,4,5,6,7,8},
			head = {1,2,3,4,5,6,7,8,9};
	
		auto g = make_graph_fragment(node_count, tail, head);

		BitVector is_source(node_count, false);
		is_source.set(0);

		BitVector is_target(node_count, false);
		is_target.set(9);

		BlockingFlow algo(g, is_source, is_target);

		while(!algo.is_finished())
			algo.advance();

		EXPECT_CMP(algo.get_current_flow_intensity(), ==, 1);
			

		{
			auto c = algo.get_source_cut();

			EXPECT_CMP(c.cut_size, ==, 1);
			EXPECT_CMP(c.node_on_side_count, ==, 1);
		
			EXPECT(c.is_node_on_side.is_set(0));
			EXPECT(!c.is_node_on_side.is_set(1));
			EXPECT(!c.is_node_on_side.is_set(2));
			EXPECT(!c.is_node_on_side.is_set(3));
			EXPECT(!c.is_node_on_side.is_set(4));
			EXPECT(!c.is_node_on_side.is_set(5));
			EXPECT(!c.is_node_on_side.is_set(6));
			EXPECT(!c.is_node_on_side.is_set(7));
			EXPECT(!c.is_node_on_side.is_set(8));
			EXPECT(!c.is_node_on_side.is_set(9));
		}

		{
			auto c = algo.get_target_cut();

			EXPECT_CMP(c.cut_size, ==, 1);
			EXPECT_CMP(c.node_on_side_count, ==, 1);
		
			EXPECT(!c.is_node_on_side.is_set(0));
			EXPECT(!c.is_node_on_side.is_set(1));
			EXPECT(!c.is_node_on_side.is_set(2));
			EXPECT(!c.is_node_on_side.is_set(3));
			EXPECT(!c.is_node_on_side.is_set(4));
			EXPECT(!c.is_node_on_side.is_set(5));
			EXPECT(!c.is_node_on_side.is_set(6));
			EXPECT(!c.is_node_on_side.is_set(7));
			EXPECT(!c.is_node_on_side.is_set(8));
			EXPECT(c.is_node_on_side.is_set(9));
		}

		{
			auto c = algo.get_balanced_cut();

			EXPECT_CMP(c.cut_size, ==, 1);
			EXPECT_CMP(c.node_on_side_count, ==, 5);
		
			bool source_side = c.is_node_on_side.is_set(0);
			bool target_side = c.is_node_on_side.is_set(9);


			EXPECT(c.is_node_on_side.is_set(0) == source_side);
			EXPECT(c.is_node_on_side.is_set(1) == source_side);
			EXPECT(c.is_node_on_side.is_set(2) == source_side);
			EXPECT(c.is_node_on_side.is_set(3) == source_side);
			EXPECT(c.is_node_on_side.is_set(4) == source_side);
			EXPECT(c.is_node_on_side.is_set(5) == target_side);
			EXPECT(c.is_node_on_side.is_set(6) == target_side);
			EXPECT(c.is_node_on_side.is_set(7) == target_side);
			EXPECT(c.is_node_on_side.is_set(8) == target_side);
			EXPECT(c.is_node_on_side.is_set(9) == target_side);
		}
	}


	{
		unsigned node_count = 4;
		std::vector<unsigned>
			tail = { 0, 0, 1, 3},
			head = { 1, 2, 3, 2};

		auto g = make_graph_fragment(node_count, tail, head);

		auto p = decompose_graph_fragment_into_connected_components(g);

		EXPECT_CMP(p.size(), ==, 1);

		EXPECT_CMP(p[0].tail.size(), ==, 8);
		EXPECT_CMP(p[0].head.size(), ==, 8);
		EXPECT_CMP(p[0].back_arc.size(), ==, 8);

		for(unsigned i=0; i<p[0].tail.size(); ++i)
			EXPECT_CMP(i, ==, p[0].back_arc[p[0].back_arc[i]]);
	}

	{
		unsigned node_count = 4;
		std::vector<unsigned>
			tail = {},
			head = {};

		auto g = make_graph_fragment(node_count, tail, head);

		auto p = decompose_graph_fragment_into_connected_components(g);

		EXPECT_CMP(p.size(), ==, 4);
		
		for(unsigned i=0; i<4; ++i){
			EXPECT_CMP(p[i].tail.size(), ==, 0);
			EXPECT_CMP(p[i].head.size(), ==, 0);
			EXPECT_CMP(p[i].back_arc.size(), ==, 0);
			EXPECT_CMP(p[i].global_node_id.size(), ==, 1);
			EXPECT_CMP(p[i].global_node_id[0], ==, i);
			
		}
	}

	{
		unsigned node_count = 4;
		std::vector<unsigned>
			tail = {0,1,2},
			head = {1,2,0};

		auto g = make_graph_fragment(node_count, tail, head);

		auto p = decompose_graph_fragment_into_connected_components(g);

		EXPECT_CMP(p.size(), ==, 2);
		EXPECT((p[0].global_node_id.size() == 3 && p[1].global_node_id.size() == 1) || (p[0].global_node_id.size() == 1 && p[1].global_node_id.size() == 3));
	}
	return expect_failed;
}
