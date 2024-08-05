#include <routingkit/osm_simple.h>

#include <iostream>
#include <stdexcept>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		string pbf_file;

		if (argc != 2) {
			cerr << argv[0] << " pbf_file" << endl;
			return 1;
		} else {
			pbf_file = argv[1];
		}

		auto routing_graph = simple_load_osm_car_routing_graph_from_pbf(pbf_file, nullptr, false, false, OSMRoadGeometry::uncompressed);
		
		cout << "{\"type\":\"FeatureCollection\",\"features\":[" << endl;

		// for all nodes in the graph
		for (unsigned x = 0; x < routing_graph.node_count(); x++) {
			// xy is the arc from node x to node y
			for(unsigned xy=routing_graph.first_out[x]; xy<routing_graph.first_out[x+1]; ++xy) {
				unsigned y = routing_graph.head[xy];

				cout << "{\"type\":\"Feature\",\"id\":\"" << xy << "\",\"properties\":{\"from\":" << x << ",\"to\":" << y << "},\"geometry\":{\"type\":\"LineString\",\"coordinates\":[" << "[" << routing_graph.longitude[x] << "," << routing_graph.latitude[x] << "]";

				for (unsigned m = routing_graph.first_modelling_node[xy]; m < routing_graph.first_modelling_node[xy + 1]; ++m) {
					cout << ",[" << routing_graph.modelling_node_longitude[m] << "," << routing_graph.modelling_node_latitude[m] << "]";
				}

				cout << ",[" << routing_graph.longitude[y] << "," << routing_graph.latitude[y] << "]" << "]}}," << endl;
			}
		}

		cout << "{}]}" << endl;
	} catch(exception&err) {
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

