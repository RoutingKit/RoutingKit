#include <routingkit/timer.h>

#include <sys/time.h>

namespace RoutingKit{

long long get_micro_time(){
	timeval t;
	gettimeofday(&t, 0);
	return t.tv_sec*1000000ll+t.tv_usec;
}

} // RoutingKit
