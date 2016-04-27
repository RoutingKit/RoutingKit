# RoutingKit

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
#include <iostream>
using namespace RoutingKit;
using namespace std;

int main(){
	// Load a car routing graph from OpenStreetMap-based data
	auto graph = simple_load_osm_car_routing_graph_from_pbf("file.pbf");
	auto tail = invert_inverse_vector(graph.first_out);

	// Build the index
	auto ch = ContractionHierarchy::build(
		graph.node_count(), 
		tail, graph.head, 
		graph.travel_time
	);

	// Besides the CH itself we need a query object. 
	ContractionHierarchyQuery ch_query(ch);

	// Use the query object to answer queries from stdin to stdout
	unsigned from, to;
	while(cin >> from >> to){
		long long start_time = get_micro_time();
		ch_query.reset().add_source(from).add_target(to).run();
		auto distance = ch_query.get_distance();
		auto path = ch_query.get_node_path();
		long long end_time = get_micro_time();

		cout << "To get from "<< from << " to "<< to << " one needs " << distance << " seconds." << endl;
		cout << "This query was answered in " << (end_time - start_time) << " microseconds." << endl;
		cout << "The path is";
		for(auto x:path)
			cout << " " << x;
		cout << endl;
	}
}
```

## Documentation

* [Setup and Installation](doc/Setup.md)
* [Support Functionality](doc/SupportFunctions.md)
* [Contraction Hierarchy](doc/ContractionHierarchy.md)
* [Customizable Contraction Hierarchy](doc/CustomizableContractionHierarchy.md)
* [OpenStreetMap Importer](doc/OpenStreetMap.md)

## Publications

* Customizable Contraction Hierarchies.
  Julian Dibbelt, Ben Strasser, and Dorothea Wagner.
  ACM Journal of Experimental Algorithmics, 2016.
* Exact Routing in Large Road Networks Using Contraction Hierarchies.
  Robert Geisberger, Peter Sanders, Dominik Schultes, and Christian Vetter.
  Transportation Science, 2012.

Please cite the first article if you use our code in a publication.
