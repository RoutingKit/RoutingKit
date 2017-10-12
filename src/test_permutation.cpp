#include <routingkit/permutation.h>

#include "expect.h"

#include <vector>
#include <random>

using namespace RoutingKit;
using namespace std;

typedef std::vector<unsigned> P;
typedef std::vector<std::string> S;

int main(){
	EXPECT(is_permutation(P{1,5,2,0,3,6,4}));
	EXPECT(!is_permutation(P{1,5,2,3,6,4}));
	EXPECT(!is_permutation(P{1,5,2,3,6,2,4}));
	EXPECT(!is_permutation(P{1,5,0,2,3,6,2,4}));

	EXPECT(is_permutation(P{}));
	EXPECT(is_permutation(P{0}));

	EXPECT(identity_permutation(0) == P{});
	EXPECT(identity_permutation(1) == P{0});
	EXPECT(identity_permutation(2) == (P{0, 1}));
	EXPECT(identity_permutation(10) == (P{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

	{
		P p = {0, 3, 1, 2};
		P inv_p = {0, 2, 3, 1};

		EXPECT(invert_permutation(p) == inv_p);
		EXPECT(invert_permutation(inv_p) == p);

		EXPECT(apply_inverse_permutation(P{}, S{}) == S{});
		EXPECT(apply_permutation(P{}, S{}) == S{});
	

		S o = {"a", "b", "c", "d"};
		S inv_p_o = {"a", "c", "d", "b"};
		S p_o = {"a", "d", "b", "c"};
	

		EXPECT(apply_permutation(p, o) == p_o);
		EXPECT(apply_permutation(inv_p, o) == inv_p_o);

		EXPECT(apply_inverse_permutation(p, o) == inv_p_o);
		EXPECT(apply_inverse_permutation(inv_p, o) == p_o);

		EXPECT(apply_permutation(p, p) == inv_p);
		EXPECT(apply_permutation(p, apply_permutation(p, p)) == identity_permutation(4));
		EXPECT(apply_permutation(inv_p, inv_p) == p);
		EXPECT(apply_permutation(apply_permutation(inv_p, inv_p), inv_p) == identity_permutation(4));
		EXPECT(apply_permutation(p, inv_p) == identity_permutation(4));
	}

	{
		auto q = random_permutation(10, std::default_random_engine(42));

		EXPECT(is_permutation(q));
		EXPECT(apply_permutation(q, invert_permutation(q)) == identity_permutation(10));
	}


	{
		auto p = random_permutation(100, std::default_random_engine(1));	
		auto q = random_permutation(100, std::default_random_engine(2));
		auto elements = random_permutation(100, std::default_random_engine(3));

		EXPECT(apply_permutation(q, apply_permutation(p, elements)) == apply_permutation(chain_permutation_first_left_then_right(p, q), elements));		
	}
	return expect_failed;
}
