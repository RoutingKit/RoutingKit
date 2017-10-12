#include <routingkit/inverse_vector.h>

#include "expect.h"

using namespace RoutingKit;
using namespace std;

void run(std::vector<unsigned>v){
	unsigned element_count = 13;

	vector<unsigned>inv_v = invert_vector(v, element_count);
	EXPECT_CMP(inv_v.size(), ==, element_count+1);

	EXPECT_CMP(inv_v.front(), ==, 0);
	EXPECT_CMP(inv_v.back(), ==, v.size());

	std::vector<bool>tested(v.size(), false);

	for(unsigned i=0; i<element_count; ++i){
		for(unsigned j=inv_v[i]; j<inv_v[i+1]; ++j){
			EXPECT_CMP(v[j], == ,i);
			tested[j] = true;
		}
	}

	for(auto x:tested){
		EXPECT(x);
		(void)x;
	}

	EXPECT(invert_inverse_vector(inv_v) == v);

	EXPECT(invert_vector({}, 3) == (vector<unsigned>{0,0,0,0}));
	EXPECT(invert_vector({}, 0) == (vector<unsigned>{0}));
	EXPECT(invert_inverse_vector(vector<unsigned>{0}) == (vector<unsigned>{}));
}

int main(){
	run({0,0,0,2,2,2,3,5,6,6,7,8,8,8,8,9});
	run({0,0,0,2,2,2,3,5,6,6,7,8,8,8,8,9,12});
	return expect_failed;
}
