#include <routingkit/timer.h>

#ifdef __unix__
#include <sys/time.h>
#else
#include <chrono>
#endif

namespace RoutingKit{

#ifdef __unix__
long long get_micro_time(){
	timeval t;
	gettimeofday(&t, 0);
	return t.tv_sec*1000000ll+t.tv_usec;
}
#else
long long get_micro_time(){
	return std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
}
#endif

} // RoutingKit
