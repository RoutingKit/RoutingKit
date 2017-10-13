#include <routingkit/min_max.h>
#include <routingkit/geo_position_to_node.h>
#include <routingkit/vector_io.h>
#include <routingkit/timer.h>
#include <routingkit/geo_dist.h>
#include <routingkit/sort.h>

#include "expect.h"
#include <iostream>
#include <random>

using namespace RoutingKit;
using namespace std;

int main(){

	const unsigned point_cloud_size = 250000;
	const unsigned query_count = 1000;
	const unsigned verify_query_count = 100;
	const double tolerance = 0.01; // 1 cm tolerance

	vector<float>latitude(point_cloud_size);
	vector<float>longitude(point_cloud_size);

	minstd_rand gen;

	auto gen_lat = [&]{
		auto lat = uniform_real_distribution<float>(-15.0, 15.0)(gen);
		if(lat < 0.0)
			lat -= 75.0;
		else
			lat += 75.0;
		return lat;
	};

	auto gen_lon = [&]{
		auto lon = uniform_real_distribution<float>(-15.0, 15.0)(gen);
		if(lon < 0.0)
			lon -= 165.0;
		else
			lon += 165.0;
		return lon;
	};
	
	for(unsigned i=0; i<point_cloud_size; ++i){
		latitude[i] = gen_lat();
		longitude[i] = gen_lon();
	}

	vector<float> query_latitude(query_count);
	vector<float> query_longitude(query_count);

	for(unsigned i=0; i<query_count; ++i){
		query_latitude[i] = gen_lat();
		query_longitude[i] = gen_lon();
	}

	long long construction_time = -get_micro_time();
	GeoPositionToNode index(latitude, longitude);
	construction_time += get_micro_time();

	vector<float>query_dist_answer(query_count);
	vector<unsigned>query_node_answer(query_count);


	cout << "Index with " << point_cloud_size << " points was construction in " << construction_time << " musec" << endl;

	float test_radii [] = {100, 1000, 10000, 100000};
	for(float radius:test_radii){
		long long query_time = -get_micro_time();
		for(unsigned i=0; i<query_count; ++i){
			auto result = index.find_nearest_neighbor_within_radius(query_latitude[i], query_longitude[i], radius);
			query_dist_answer[i] = result.distance;
			query_node_answer[i] = result.id;

		}
		query_time += get_micro_time();

		cout << query_count <<" queries were answered in " << query_time << " musec with radius "<< radius << endl;
		cout << "per query running time : " << (double)query_time / (double)query_count << "musec" << endl;

		unsigned non_trivial_count = 0;
		
		for(unsigned i=0; i<query_count; ++i){
			if(query_node_answer[i] != invalid_id){
				++non_trivial_count;
				EXPECT_CMP(query_node_answer[i], <, point_cloud_size);

				auto actual_distance = geo_dist(query_latitude[i], query_longitude[i], latitude[query_node_answer[i]], longitude[query_node_answer[i]]);

				EXPECT_CMP(fabs(actual_distance - query_dist_answer[i]), <=, tolerance);
			}
		}

		cout << "finished testing all query answers for plausibility" << endl;
		cout << "of "<< query_count <<" queries there were "<< non_trivial_count << " queries with a non-trivial answer" << endl;

		for(unsigned i=0; i<verify_query_count; ++i){
			float min_dist;
			if(query_node_answer[i] != invalid_id)
				min_dist = query_dist_answer[i];
			else
				min_dist = radius;

			for(unsigned j=0; j<point_cloud_size; ++j){
				EXPECT_CMP(
					geo_dist(latitude[j], longitude[j], query_latitude[i], query_longitude[i]), >=, min_dist - tolerance
				);
			}
		}
	
		cout << "verified the first "<< verify_query_count << " queries" << endl;

	}


	vector<vector<GeoPositionToNode::NearestNeighborhoodQueryResult>>query_answer(query_count);

	vector<bool>in_set(point_cloud_size, false);

	for(float radius:test_radii){
		long long query_time = -get_micro_time();
		for(unsigned i=0; i<query_count; ++i){
			query_answer[i] = index.find_all_nodes_within_radius(query_latitude[i], query_longitude[i], radius);
		}
		query_time += get_micro_time();

		cout << query_count <<" queries were answered in " << query_time << " musec with radius "<< radius << " m" << endl;
		cout << "per query running time : " << (double)query_time / (double)query_count << "musec" << endl;

		long long size_sum = 0;
		for(unsigned i=0; i<query_count; ++i){
			size_sum += query_answer[i].size();
			for(auto x:query_answer[i]){
				EXPECT_CMP(x.id, !=, invalid_id);
				EXPECT_CMP(x.id, <, point_cloud_size);

				auto actual_distance = geo_dist(query_latitude[i], query_longitude[i], latitude[x.id], longitude[x.id]);

				EXPECT_CMP(fabs(actual_distance - x.distance), <=, tolerance);
			}
		}
		cout << "average returned point set size : " << (double)size_sum / (double)query_count << endl;

		cout << "tested all queries for whether all returned points are within range" << endl;


		for(unsigned i=0; i<verify_query_count; ++i){

			for(auto x:query_answer[i])
				in_set[x.id] = true;

			for(unsigned j=0; j<point_cloud_size; ++j){
				if(geo_dist(latitude[j], longitude[j], query_latitude[i], query_longitude[i]) < radius - tolerance){
					EXPECT(in_set[j]);
				}
			}

			for(auto x:query_answer[i])
				in_set[x.id] = false;
		}

		cout << "fully verified the first "<< verify_query_count << " queries" << endl;

	}
	return expect_failed;
}
