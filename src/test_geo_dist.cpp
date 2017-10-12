#include <iostream>
#include <cmath>
#include <random>
#include <routingkit/timer.h>
#include <routingkit/geo_dist.h>
#include "expect.h"
using namespace RoutingKit;
using namespace std;

// Code based on formula from http://www.movable-type.co.uk/scripts/latlong.html
// This code was in place for quite some time until it was replaced by the current code in <routingkit/geo_dist.h>
// They should do the same thing. We use the old code to verify the new code.

double old_geo_dist(double a_lat, double a_lon, double b_lat, double b_lon){
	const double pi = 3.14159265359;
	const double R = 6371000.785; // earth radius in meter
	const double inv_180 = 1.0 / 180;

	a_lat *= inv_180;
	a_lat *= pi;
	b_lat *= inv_180;
	b_lat *= pi;
	a_lon *= inv_180;
	a_lon *= pi;
	b_lon *= inv_180;
	b_lon *= pi;

	double dlat = b_lat - a_lat;
	double dlon = b_lon - a_lon;

	double a_ = sin(dlat*0.5) * sin(dlat*0.5) + sin(dlon*0.5) * sin(dlon*0.5) * cos(a_lat) * cos(b_lat);
	double c = 2 * atan2(sqrt(a_), sqrt(1-a_));
	return R * c;
}

int main(){
	

	std::minstd_rand rand_gen;

	std::uniform_real_distribution<double>lat_dist(-90.0, 90.0);
	std::uniform_real_distribution<double>lon_dist(-180, 180.0);

	unsigned test_count = 10000000;
	vector<double>
		lat_a(test_count),
		lon_a(test_count),
		lat_b(test_count),
		lon_b(test_count);

	for(auto&x:lat_a)
		x = lat_dist(rand_gen);
	for(auto&x:lat_b)
		x = lat_dist(rand_gen);
	for(auto&x:lon_a)
		x = lon_dist(rand_gen);
	for(auto&x:lon_b)
		x = lon_dist(rand_gen);

	lat_a[0] = 0;
	lat_b[0] = 0;

	lon_a[1] = 0;
	lon_b[1] = 0;

	lat_a[2] = 0;
	lat_b[2] = 0;
	lon_a[2] = 0;
	lon_b[2] = 0;

	lat_a[3] = 42;
	lat_b[3] = 42;

	lon_a[4] = 42;
	lon_b[4] = 42;

	lat_a[5] = 42;
	lat_b[5] = 42;
	lon_a[5] = 42;
	lon_b[5] = 42;

	lat_a[6] = -42;
	lat_b[6] = -42;

	lon_a[7] = -42;
	lon_b[7] = -42;

	lat_a[8] = -42;
	lat_b[8] = -42;
	lon_a[8] = -42;
	lon_b[8] = -42;

	lat_a[9] = lat_b[9];
	lon_a[9] = lon_b[9];

	lat_a[10] = 49.014139;
	lon_a[10] = 8.404696;
	lat_b[10] = 49.014192;
	lon_b[10] = 8.404806;

	lat_a[11] = 68.951798;
	lon_a[11] = 23.110359;
	lat_b[11] = 65.777244;
	lon_b[11] = -155.834878;

	vector<double>
		old_dist_result(test_count),
		new_dist_result(test_count);

	long long old_timer = -get_micro_time();
	for(unsigned i=0; i<test_count; ++i){
		old_dist_result[i] = old_geo_dist(lat_a[i], lon_a[i], lat_b[i], lon_b[i]);
	}
	old_timer += get_micro_time();

	long long new_timer = -get_micro_time();
	for(unsigned i=0; i<test_count; ++i){
		new_dist_result[i] = geo_dist(lat_a[i], lon_a[i], lat_b[i], lon_b[i]);
	}
	new_timer += get_micro_time();

	EXPECT_CMP(fabs(new_dist_result[10]-9.99), <, 0.1);
	EXPECT_CMP(fabs(new_dist_result[11]-5033760), <, 100);

	double max_diff = 0.0;
	for(unsigned i=0; i<test_count; ++i){
		double diff = std::fabs(old_dist_result[i] - new_dist_result[i]);
		if(diff > max_diff)
			max_diff = diff;
	}

	EXPECT_CMP(fabs(max_diff), <, 0.001);

	cout << "running time of old geo distance for "<<test_count << " tests : " << old_timer << " musec" << endl;
	cout << "running time of new geo distance for "<<test_count << " tests : " << new_timer << " musec" << endl;

	return expect_failed;
}
