#include <routingkit/contraction_hierarchy.h>
#include <routingkit/customizable_contraction_hierarchy.h>
#include <routingkit/osm_graph_builder.h>
#include <routingkit/osm_simple.h>
#include <routingkit/nested_dissection.h>
#include <routingkit/timer.h>

#include "expect.h"

#include <iostream>
#include <random>


using namespace std;
using namespace RoutingKit;

void log_message(const std::string&msg){
	cout << msg << endl;
}

int main(int argc, char*argv[]){

	string pbf_file;

	if(argc == 2){
		pbf_file = argv[1];
	} else {
		cout
			<< "Usage: "<< argv[0] << " test.pbf\n"
			<< "Example: \n"
			<< "wget https://download.geofabrik.de/europe/germany/baden-wuerttemberg-latest.osm.pbf\n"
			<< argv[0] << " baden-wuerttemberg-latest.osm.pbf" << endl;
		return 1;
	}

	cout << "Test car without logging" << endl;
	simple_load_osm_car_routing_graph_from_pbf(pbf_file);
	cout << "Done" << endl;

	cout << "Test car with logging" << endl;
	simple_load_osm_car_routing_graph_from_pbf(pbf_file, log_message);
	cout << "Done" << endl;

	cout << "Test car with logging and degree 2 nodes" << endl;
	simple_load_osm_car_routing_graph_from_pbf(pbf_file, log_message, true);
	cout << "Done" << endl;


	cout << "Test pedestrian without logging" << endl;
	simple_load_osm_pedestrian_routing_graph_from_pbf(pbf_file);
	cout << "Done" << endl;

	cout << "Test pedestrian with logging" << endl;
	simple_load_osm_pedestrian_routing_graph_from_pbf(pbf_file, log_message);
	cout << "Done" << endl;

	cout << "Test pedestrian with logging and degree 2 nodes" << endl;
	simple_load_osm_pedestrian_routing_graph_from_pbf(pbf_file, log_message, true);
	cout << "Done" << endl;


	cout << "Test bicycle without logging" << endl;
	simple_load_osm_bicycle_routing_graph_from_pbf(pbf_file);
	cout << "Done" << endl;

	cout << "Test bicycle with logging" << endl;
	simple_load_osm_bicycle_routing_graph_from_pbf(pbf_file, log_message);
	cout << "Done" << endl;

	cout << "Test bicycle with logging and degree 2 nodes" << endl;
	simple_load_osm_bicycle_routing_graph_from_pbf(pbf_file, log_message, true);
	cout << "Done" << endl;

}
