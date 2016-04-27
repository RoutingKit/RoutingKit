#include <routingkit/vantage_point_tree.h>
#include <routingkit/geo_dist.h>
#include <routingkit/constants.h>
#include <vector>
#include <algorithm>

#include <math.h>
#include <assert.h>

// See https://en.wikipedia.org/wiki/Vantage-point_tree for details

namespace RoutingKit{

namespace{
	float compute_distance(VantagePointTree::PointPosition x, VantagePointTree::PointPosition y){
		return geo_dist(x.latitude, x.longitude, y.latitude, y.longitude);
	}

	struct PointData{
		VantagePointTree::PointPosition position;
		unsigned id;
		float distance_to_pivot;
	};

	const unsigned max_points_per_leaf = 8;

	void construct_tree(
		std::vector<PointData>&d,
		unsigned begin, unsigned end){
		if(end - begin > max_points_per_leaf){


			auto mid = begin + (end - begin)/2;

			std::nth_element(
				d.begin() + begin+1, d.begin() + mid, d.begin() + end, 
				[](PointData l, PointData r){
					return l.distance_to_pivot < r.distance_to_pivot;
				}
			);

			construct_tree(d, begin, mid);
			for(auto i=mid; i!=end; ++i)
				d[i].distance_to_pivot = compute_distance(d[mid].position, d[i].position);
			construct_tree(d, mid, end);
		}
	}
}

VantagePointTree::VantagePointTree(const std::vector<float>&latitude, const std::vector<float>&longitude):
	point_position(latitude.size()), point_id(latitude.size()){
	unsigned point_count = latitude.size();

	std::vector<PointData>data(point_count);

	for(unsigned i=0; i<point_count; ++i){
		data[i].position = {latitude[i],longitude[i]};
		data[i].id = i;
		data[i].distance_to_pivot = compute_distance(data[0].position, data[i].position);
	}
	construct_tree(data, 0, point_count);
	for(unsigned i=0; i<point_count; ++i){
		point_position[i] = data[i].position;
		point_id[i] = data[i].id;
	}
}

namespace{
	// I envy the day that C++ will finally support recursive lambda functions...

	void nearest_neighbor_recursion(
		const std::vector<VantagePointTree::PointPosition>&point_position, const std::vector<unsigned>&point_id, 
		unsigned begin, unsigned end, 
		VantagePointTree::PointPosition query_position,
		VantagePointTree::NearestNeighborhoodQueryResult&current_result
	){
		if(end - begin <= max_points_per_leaf){
			for(unsigned i=begin; i<end; ++i){
				auto distance = compute_distance(query_position, point_position[i]);
				if(distance <= current_result.distance)
					current_result = {point_id[i], distance};
			}
		}else{
			auto recurse = [&](unsigned new_begin, unsigned new_end){
				nearest_neighbor_recursion(point_position, point_id, new_begin, new_end, query_position, current_result);
			};

			auto pivot_position = point_position[begin];

			unsigned mid = begin + (end - begin)/2;
			auto pivot_query_distance = compute_distance(pivot_position, query_position);
			auto pivot_boundary_distance = compute_distance(pivot_position, point_position[mid]);
			
//			#ifndef NDEBUG
//			for(unsigned i=begin; i<mid; ++i)
//				assert(compute_distance(pivot_position, point_position[i]) <= pivot_boundary_distance);
//			for(unsigned i=mid+1; i<end; ++i)
//				assert(compute_distance(pivot_position, point_position[i]) >= pivot_boundary_distance);
//			#endif

			if(pivot_query_distance >= pivot_boundary_distance){
				recurse(mid, end);
				if(pivot_query_distance - current_result.distance <= pivot_boundary_distance)
					recurse(begin, mid);
			}else{
				recurse(begin, mid);
				if(pivot_query_distance + current_result.distance >= pivot_boundary_distance)
					recurse(mid, end);
			}
			
		}
	}
}

VantagePointTree::NearestNeighborhoodQueryResult VantagePointTree::find_nearest_neighbor_within_radius(float query_latitude, float query_longitude, float query_radius)const{
	NearestNeighborhoodQueryResult result = {invalid_id, query_radius};
	nearest_neighbor_recursion(point_position, point_id, 0, point_count(), {query_latitude, query_longitude}, result);
	return result;
}

} // RoutingKit
