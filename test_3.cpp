#include <routingkit/osm_simple.h>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/timer.h>
#include <routingkit/geo_position_to_node.h>
#include <iostream>
using namespace RoutingKit;
using namespace std;

int pitcairn(){
	// Load a car routing graph from OpenStreetMap-based data
	auto graph = simple_load_osm_car_routing_graph_from_pbf("pitcairn.pbf");
	auto tail = invert_inverse_vector(graph.first_out);

	// Build the shortest path index
	auto ch = ContractionHierarchy::build(
		graph.node_count(),
		tail, graph.head,
		graph.travel_time
	);

	// Build the index to quickly map latitudes and longitudes
	GeoPositionToNode map_geo_position(graph.latitude, graph.longitude);

	// Besides the CH itself we need a query object.
	ContractionHierarchyQuery ch_query(ch);

	// Use the query object to answer queries from stdin to stdout
	float from_latitude, from_longitude, to_latitude, to_longitude;
	from_latitude = -25.068391;
	from_longitude = -130.117748;
	to_latitude = -25.068514;
	to_longitude = -130.100376;
	unsigned from = map_geo_position.find_nearest_neighbor_within_radius(from_latitude, from_longitude, 1000).id;
	if(from == invalid_id){
		cout << "No node within 1000m from source position" << endl;
		//continue;
	}
	unsigned to = map_geo_position.find_nearest_neighbor_within_radius(to_latitude, to_longitude, 1000).id;
	if(to == invalid_id){
		cout << "No node within 1000m from target position" << endl;
		//continue;
	}

	long long start_time = get_micro_time();
	ch_query.reset().add_source(from).add_target(to).run();
	auto distance = ch_query.get_distance();
	auto path = ch_query.get_node_path();
	long long end_time = get_micro_time();

	cout << "The path is";
	for(auto x:path)
		cout << " " << x;

	return 0;
}