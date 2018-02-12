#include <routingkit/parse_duration.h>
#include "expect.h"

using namespace RoutingKit;
using namespace std;

int main(){
	// ISO 8601.
	EXPECT_CMP(parse_duration("P4D"), ==, 4*24*60*60);
	EXPECT_CMP(parse_duration("PT71M"), ==, 71*60);
	EXPECT_CMP(parse_duration("PT5M"), ==, 5*60);
	// Simple times.
	EXPECT_CMP(parse_duration("15"), ==, 15*60);
	EXPECT_CMP(parse_duration("00:15"), ==, 15*60);
	EXPECT_CMP(parse_duration("00:15:00"), ==, 15*60);
	EXPECT_CMP(parse_duration("05:01"), ==, 5*60*60 + 1*60);
	// Default 10 minutes.
	EXPECT_CMP(parse_duration("xxx"), ==, 10 * 60);
	return expect_failed;
}
