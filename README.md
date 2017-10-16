# RoutingKit

[![Build Status](https://travis-ci.org/RoutingKit/RoutingKit.svg?branch=master)](https://travis-ci.org/RoutingKit/RoutingKit)

RoutingKit is a C++ library that provides advanced route planning functionality. 
It was developed at [KIT](https://www.kit.edu) in the [group of Prof. Dorothea Wagner](https://i11www.iti.kit.edu/).
The most prominent component is an index-based data structure called (Customizable) Contraction Hierarchy, that allows to answer shortest path queries within milliseconds or even less on data sets of continental size while keeping the arc weights flexible.
Such running times cannot be achieved without indices.

One of the main design goals of RoutingKit is to make recent research results easily accessible to people developing route planning applications.
A key element is an interface that is a good compromise between usability and running time performance.
For example the following code snippet is enough to build and query a basic index given an [OSM](https://www.openstreetmap.org) PBF data export.

```cpp
#include <routingkit/osm_simple.h>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/timer.h>
#include <routingkit/geo_position_to_node.h>
#include <iostream>
using namespace RoutingKit;
using namespace std;

int main(){
	// Load a car routing graph from OpenStreetMap-based data
	auto graph = simple_load_osm_car_routing_graph_from_pbf("file.pbf");
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
	while(cin >> from_latitude >> from_longitude >> to_latitude >> to_longitude){
		unsigned from = map_geo_position.find_nearest_neighbor_within_radius(from_latitude, from_longitude, 1000).id;
		if(from == invalid_id){
			cout << "No node within 1000m from source position" << endl;
			continue;
		}
		unsigned to = map_geo_position.find_nearest_neighbor_within_radius(to_latitude, to_longitude, 1000).id;
		if(to == invalid_id){
			cout << "No node within 1000m from target position" << endl;
			continue;
		}

		long long start_time = get_micro_time();
		ch_query.reset().add_source(from).add_target(to).run();
		auto distance = ch_query.get_distance();
		auto path = ch_query.get_node_path();
		long long end_time = get_micro_time();

		cout << "To get from "<< from << " to "<< to << " one needs " << distance << " milliseconds." << endl;
		cout << "This query was answered in " << (end_time - start_time) << " microseconds." << endl;
		cout << "The path is";
		for(auto x:path)
			cout << " " << x;
		cout << endl;
	}
}
```

You can get OSM PBF exports from various sources. A popular one is [Geofabrik](https://download.geofabrik.de/). Another one is [Mapzen](https://mapzen.com/data/metro-extracts/).

## Documentation

The functions of RoutingKit are made available using different headers. This allows for a fine-grained control over what is made available and what not. The exact headers are available from the documentation. For cases where this fine-grained control is not necessary, RoutingKit provides the `<routingkit/all.h>` header, which includes everything. All functionality is in the `RoutingKit` namespace.

* [Setup and Installation](doc/Setup.md)
* [Support Functionality](doc/SupportFunctions.md)
* [Contraction Hierarchy](doc/ContractionHierarchy.md)
* [Customizable Contraction Hierarchy](doc/CustomizableContractionHierarchy.md)
* [OpenStreetMap Importer](doc/OpenStreetMap.md)
* [Converting Coordinates to Node ID](doc/CoordinatesToNodeID.md)

## Publications

Please cite the following article if you use our code in a publication:

* Customizable Contraction Hierarchies.
  Julian Dibbelt, Ben Strasser, and Dorothea Wagner.
  ACM Journal of Experimental Algorithmics, 2016.

The original CH algorithm was introduced in:

* Exact Routing in Large Road Networks Using Contraction Hierarchies.
  Robert Geisberger, Peter Sanders, Dominik Schultes, and Christian Vetter.
  Transportation Science, 2012.


