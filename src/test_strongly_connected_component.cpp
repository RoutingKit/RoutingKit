#include <routingkit/strongly_connected_component.h>
#include <routingkit/inverse_vector.h>

#include "expect.h"

#include <vector>
#include <random>

using namespace RoutingKit;
using namespace std;

typedef std::vector<unsigned> P;
typedef std::vector<std::string> S;

int main(){
	{
		std::vector<unsigned>first_out = {0};
		std::vector<unsigned>head;

		StronlyConnectedComponentsResult scc = compute_strongly_connected_components(first_out, head);
		EXPECT(scc.component_of.empty());
		EXPECT(scc.component_count == 0);
	}

	{
		const unsigned node_count = 11;
		std::vector<unsigned>tail = {0, 1, 2, 2, 2, 3, 4, 4, 5, 5, 6, 7, 8, 9};
		std::vector<unsigned>head = {1, 2, 1, 3, 4, 2, 5, 8, 2, 6, 7, 9, 8, 6};
		std::vector<unsigned>first_out = invert_vector(tail, node_count);

		StronlyConnectedComponentsResult scc = compute_strongly_connected_components(first_out, head);
		EXPECT(scc.component_of.size() == node_count);
		EXPECT(scc.component_count == 5);

		EXPECT(scc.component_of[1] == scc.component_of[2]);
		EXPECT(scc.component_of[1] == scc.component_of[3]);
		EXPECT(scc.component_of[1] == scc.component_of[4]);
		EXPECT(scc.component_of[1] == scc.component_of[5]);

		EXPECT(scc.component_of[6] == scc.component_of[7]);
		EXPECT(scc.component_of[6] == scc.component_of[9]);

		EXPECT(scc.component_of[0] != scc.component_of[1]);
		EXPECT(scc.component_of[0] != scc.component_of[6]);
		EXPECT(scc.component_of[0] != scc.component_of[8]);
		EXPECT(scc.component_of[0] != scc.component_of[10]);

		EXPECT(scc.component_of[1] != scc.component_of[6]);
		EXPECT(scc.component_of[1] != scc.component_of[8]);
		EXPECT(scc.component_of[1] != scc.component_of[10]);

		EXPECT(scc.component_of[6] != scc.component_of[8]);
		EXPECT(scc.component_of[6] != scc.component_of[10]);

		EXPECT(scc.component_of[8] != scc.component_of[10]);

		std::vector<bool> in_large_scc = compute_largest_strongly_connected_component(first_out, head);
		EXPECT(in_large_scc.size() == node_count);
		EXPECT(!in_large_scc[0]);
		EXPECT(in_large_scc[1]);
		EXPECT(in_large_scc[2]);
		EXPECT(in_large_scc[3]);
		EXPECT(in_large_scc[4]);
		EXPECT(in_large_scc[5]);
		EXPECT(!in_large_scc[6]);
		EXPECT(!in_large_scc[7]);
		EXPECT(!in_large_scc[8]);
		EXPECT(!in_large_scc[9]);
		EXPECT(!in_large_scc[10]);
	}

	return expect_failed;
}
