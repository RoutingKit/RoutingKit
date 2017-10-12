#include <routingkit/geo_position_to_node.h>
#include <routingkit/geo_dist.h>
#include <routingkit/constants.h>
#include <vector>
#include <algorithm>

#include <math.h>
#include <assert.h>

// See https://en.wikipedia.org/wiki/Vantage-point_tree for details

namespace RoutingKit{

namespace{
	float compute_distance(GeoPositionToNode::PointPosition x, GeoPositionToNode::PointPosition y){
		return geo_dist(x.latitude, x.longitude, y.latitude, y.longitude);
	}

	struct PointData{
		GeoPositionToNode::PointPosition position;
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

			// Slower but simpler code			
			// for(auto i=mid; i!=end; ++i)
			//	d[i].distance_to_pivot = compute_distance(d[mid].position, d[i].position);

			// Faster more complex vectorized code
			{
				const float pi = 3.14159265359;
				const float R = 6371000.785; // earth radius in meter
				const float inv_180 = 1.0 / 180;

				float a_lat = d[mid].position.latitude;
				float a_lon = d[mid].position.longitude;

				a_lat *= inv_180;
				a_lat *= pi;
				a_lon *= inv_180;
				a_lon *= pi;

				const unsigned vector_width = 16;
				for(unsigned i=mid; i<end; i+=vector_width){

					float b_lat[vector_width];
					float b_lon[vector_width];
					float a_[vector_width];

					unsigned element_count = vector_width;
					if(end - i < element_count)
						element_count = end - i;

					for(unsigned j=0; j<element_count; ++j){
						b_lat[j] = d[i+j].position.latitude;
						b_lon[j] = d[i+j].position.longitude;
					}

					for(unsigned j=element_count; j<vector_width; ++j){
						b_lat[j] = 0;
						b_lon[j] = 0;
					}

					// GCC5's auto vectorizer is clever enough to vectorize this loop 
					// OpenMP does not harm but is not needed
					// #pragma omp simd
					for(unsigned j=0; j<vector_width; ++j){
						b_lat[j] *= inv_180;
						b_lat[j] *= pi;
						b_lon[j] *= inv_180;
						b_lon[j] *= pi;

						float dlat = b_lat[j] - a_lat;
						float dlon = b_lon[j] - a_lon;

						a_[j] = sinf(dlat*0.5) * sinf(dlat*0.5) + sinf(dlon*0.5) * sinf(dlon*0.5) * cosf(a_lat) * cosf(b_lat[j]);
					}

					for(unsigned j=0; j<element_count; ++j){
						// GCC5 does not have vectorized versions of atan2f nor sqrtf
						// we therefore call the sequential functions as they would prevent the vectorization of the previous loop
						float c = 2 * atan2f(sqrtf(a_[j]), sqrtf(1-a_[j]));
						d[i+j].distance_to_pivot = R * c;
					}
				}

			}
	
			construct_tree(d, mid, end);
		}
	}
}

GeoPositionToNode::GeoPositionToNode(const std::vector<float>&latitude, const std::vector<float>&longitude):
	point_position(latitude.size()), point_id(latitude.size()){
	assert(latitude.size() == longitude.size());
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
		const std::vector<GeoPositionToNode::PointPosition>&point_position, const std::vector<unsigned>&point_id,
		unsigned begin, unsigned end,
		GeoPositionToNode::PointPosition query_position,
		GeoPositionToNode::NearestNeighborhoodQueryResult&current_result
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
				if(pivot_query_distance - pivot_boundary_distance < current_result.distance)
					recurse(begin, mid);
			}else{
				recurse(begin, mid);
				if(pivot_boundary_distance - pivot_query_distance < current_result.distance)
					recurse(mid, end);
			}
			
		}
	}


	void find_all_nodes_recursion(
		const std::vector<GeoPositionToNode::PointPosition>&point_position, const std::vector<unsigned>&point_id,
		unsigned begin, unsigned end,
		GeoPositionToNode::PointPosition query_position,
		float query_radius,
		std::vector<GeoPositionToNode::NearestNeighborhoodQueryResult>&result
	){
		if(end - begin <= max_points_per_leaf){
			for(unsigned i=begin; i<end; ++i){
				auto distance = compute_distance(query_position, point_position[i]);
				if(distance <= query_radius)
					result.push_back({point_id[i], distance});
			}
		}else{
			auto recurse = [&](unsigned new_begin, unsigned new_end){
				find_all_nodes_recursion(point_position, point_id, new_begin, new_end, query_position, query_radius, result);
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

			if(pivot_query_distance - pivot_boundary_distance <= query_radius)
				recurse(begin, mid);
			if(pivot_boundary_distance - pivot_query_distance <= query_radius)
				recurse(mid, end);
		}
	}
}

GeoPositionToNode::NearestNeighborhoodQueryResult GeoPositionToNode::find_nearest_neighbor_within_radius(float query_latitude, float query_longitude, float query_radius)const{
	assert(query_radius >= 0.0 && "radius must be positive");
	NearestNeighborhoodQueryResult result = {invalid_id, query_radius};
	nearest_neighbor_recursion(point_position, point_id, 0, point_count(), {query_latitude, query_longitude}, result);
	return result;
}

std::vector<GeoPositionToNode::NearestNeighborhoodQueryResult> GeoPositionToNode::find_all_nodes_within_radius(float query_latitude, float query_longitude, float query_radius)const{
	assert(query_radius >= 0.0 && "radius must be positive");
	std::vector<NearestNeighborhoodQueryResult> result;
	find_all_nodes_recursion(point_position, point_id, 0, point_count(), {query_latitude, query_longitude}, query_radius, result);
	return result; // NVRO
}

} // RoutingKit
