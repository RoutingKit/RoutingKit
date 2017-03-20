#include <routingkit/osm_simple.h>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/inverse_vector.h>
#include <iostream>

using namespace RoutingKit;
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << argv[0] << " << filename.pbf >> << output.index >>\n";
        exit(-1);
    }

    // Load a car routing graph from OpenStreetMap-based data
    auto graph = simple_load_osm_car_routing_graph_from_pbf(argv[1]);
    auto tail = invert_inverse_vector(graph.first_out);

    // Build the index
    auto ch = ContractionHierarchy::build(
        graph.node_count(),
        tail, graph.head,
        graph.travel_time
    );

    ch.save_file(argv[2]);
}
