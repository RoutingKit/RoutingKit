#include <routingkit/basic_api.h>

namespace RoutingKit {

int init_routingkit_ch(std::string pbf_file)
{

    bool print_log = true;
    auto log_message = print_log ? [](const std::string& msg){std::cout << msg << std::endl;} : nullptr;
    bool cold_start = false;
    std::string ch_file;

    try {
        ch_file = pbf_file.substr(0, pbf_file.find_last_of('.')) + ".ch";
        ContractionHierarchy::load_file(ch_file);
        log_message("Reuse the saved CH data of given pbf file.");
    } catch (const std::runtime_error& e) {
        log_message(e.what());
        cold_start = true;
    }

    if (cold_start) {
        // Load a routing graph from OpenStreetMap-based data
        log_message("1) Test load_osm w/ logging");

        /* simple ver. [begin] */
        //auto graph = simple_load_osm_car_routing_graph_from_pbf(pbf_file, log_message);
        //auto tail = invert_inverse_vector(graph.first_out);
        /* simple ver. [end] */
	
        /* standard ver. [begin] */
        std::vector<unsigned> first_out, tail, head, travel_time; // travel_time: in secs
        std::vector<float> latitude, longitude;

	    {
		    auto mapping = load_osm_id_mapping_from_pbf(pbf_file, nullptr,
			                [&](uint64_t osm_way_id, const TagMap& tags){
				                return is_osm_way_used_by_cars(osm_way_id, tags, log_message);
			                }, log_message);

            std::vector<uint32_t> way_speed(mapping.is_routing_way.population_count()); // in km/h

		    auto routing_graph = load_osm_routing_graph_from_pbf(pbf_file, mapping,
			                    [&](uint64_t osm_way_id, unsigned routing_way_id, const TagMap& way_tags){
				                    way_speed[routing_way_id] = get_osm_way_speed(osm_way_id, way_tags, log_message);
				                    return get_osm_car_direction_category(osm_way_id, way_tags, log_message);
			                    }, nullptr, log_message, false);

		    first_out = move(routing_graph.first_out);
		    tail = invert_inverse_vector(first_out);
		    head = move(routing_graph.head);
		    latitude = move(routing_graph.latitude);
		    longitude = move(routing_graph.longitude);
		    travel_time = move(routing_graph.geo_distance);

		    // routing_graph.geo_distance is in meter
		    for(unsigned a = 0; a < travel_time.size(); ++a){
			    travel_time[a] *= 18;
			    travel_time[a] /= way_speed[routing_graph.way[a]];
			    travel_time[a] /= 5;
		    }
	    }
        /* standard ver. [end] */
        log_message("1) Done");

        // Build the shortest path index
        log_message("2) Build CH");
        auto ch = ContractionHierarchy::build(first_out.size()-1, tail, head, travel_time, log_message);
        log_message("2) Done");

        log_message("3) Save CH as a file");
        ch.save_file(ch_file);
        log_message("3) Done");
    }

    return 0;
}

}
