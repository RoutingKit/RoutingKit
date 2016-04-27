#include <routingkit/osm_profile.h>
#include <routingkit/osm_graph_builder.h>
#include <routingkit/vector_io.h>
#include <routingkit/timer.h>
#include <routingkit/tag_map.h>

#include <iostream>
#include <string>
#include <exception>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){
	try {
		std::string 
			pbf_file,
			first_out_file,
			head_file,
			geo_distance_file,
			travel_time_file,
			way_file,
			way_speed_file,
			way_name_file,
			latitude_file,
			longitude_file,
			osm_node_file,
			osm_way_file;

		if(argc != 13){
			cout << argv[0] << " pbf_file first_out head geo_distance travel_time way way_speed way_name latitude longitude osm_node osm_way\ngeo_distance is in [m]\ntravel_time is in [s]\nway_speed is in [km/h]" << endl;
			return 1;
		}else{
			pbf_file = argv[1];
			first_out_file = argv[2];
			head_file = argv[3];
			geo_distance_file = argv[4];
			travel_time_file = argv[5];
			way_file = argv[6];
			way_speed_file = argv[7];
			way_name_file = argv[8];
			latitude_file = argv[9];
			longitude_file = argv[10];
			osm_node_file = argv[11];
			osm_way_file = argv[12];;
		}


		std::function<void(const std::string&)>log_message = [](const string&msg){
			cout << msg << endl;
		};



		auto mapping = load_osm_id_mapping_from_pbf(
			pbf_file,
			nullptr,
			[&](uint64_t osm_way_id, const TagMap&tags){
				return is_osm_way_used_by_cars(osm_way_id, tags, log_message);
			},
			log_message
		);

		unsigned routing_way_count = mapping.is_routing_way.population_count();
		std::vector<uint32_t>way_speed(routing_way_count);
		std::vector<std::string>way_name(routing_way_count);

		auto routing_graph = load_osm_routing_graph_from_pbf(
			pbf_file,
			mapping,
			[&](uint64_t osm_way_id, unsigned routing_way_id, const TagMap&way_tags){
				way_speed[routing_way_id] = get_osm_way_speed(osm_way_id, way_tags, log_message);
				way_name[routing_way_id] = get_osm_way_name(osm_way_id, way_tags, log_message);
				return get_osm_car_direction_category(osm_way_id, way_tags, log_message);
			},
			log_message,
			true
		);

		std::vector<uint32_t>travel_time = routing_graph.geo_distance;
		for(unsigned a=0; a<travel_time.size(); ++a){
			travel_time[a] *= 18;
			travel_time[a] /= way_speed[routing_graph.way[a]];
			travel_time[a] /= 5;
		}

		{
			log_message("Start saving routing graph");
			long long timer = -get_micro_time();

			save_vector(first_out_file, routing_graph.first_out);
			save_vector(head_file, routing_graph.head);
			save_vector(geo_distance_file, routing_graph.geo_distance);
			save_vector(travel_time_file, travel_time);
			save_vector(way_file, routing_graph.way);
			save_vector(way_name_file, way_name);
			save_vector(way_speed_file, way_speed);
			save_vector(latitude_file, routing_graph.latitude);
			save_vector(longitude_file, routing_graph.longitude);
			save_bit_vector(osm_node_file, mapping.is_routing_node);
			save_bit_vector(osm_way_file, mapping.is_routing_way);

			timer += get_micro_time();
			log_message("Finished saving, needed "+std::to_string(timer)+"musec.");
		}
	} catch(std::exception&err) {
		cout << "Exception : "<< err.what() << endl;
	}
}
