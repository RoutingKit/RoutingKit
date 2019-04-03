#include <routingkit/basic_api.h>

namespace RoutingKit {

GeoPositionToNode* gpn = nullptr;
ContractionHierarchy* ch = nullptr;
ContractionHierarchyQuery* ch_query = nullptr;

GeoPositionToNode* get_gpn() { return gpn; };
ContractionHierarchy* get_ch() { return ch; };
ContractionHierarchyQuery* get_ch_query() { return ch_query; };

int init_routingkit(std::string pbf_file, bool cold_start, bool print_log)
{
    auto log_message = print_log ? [](const std::string& msg){std::cout << msg << std::endl;} : nullptr;
    std::vector<float> latitude, longitude;
    auto basic_path = pbf_file.substr(0, pbf_file.find_last_of('.'));

    if (!cold_start) {
        try {
            ch = new ContractionHierarchy(ContractionHierarchy::load_file(basic_path + ".ch"));
            log_message("Reuse the saved CH data of given pbf file.");

            std::ifstream in_lat(basic_path + ".lat");
            std::ifstream in_lng(basic_path + ".lng");
            std::copy(std::istream_iterator<float>(in_lat),
                        std::istream_iterator<float>(), std::back_inserter(latitude));
            std::copy(std::istream_iterator<float>(in_lng),
                        std::istream_iterator<float>(), std::back_inserter(longitude));

            gpn = new GeoPositionToNode(latitude, longitude);

            return 0;
        } catch (const std::runtime_error& e) {
            log_message(e.what());
        }
    }

    // Load a routing graph from OpenStreetMap-based data
    log_message("Load osm data w/ logging");
    auto routing_graph = simple_load_osm_car_routing_graph_from_pbf(pbf_file, log_message);
    auto first_out = move(routing_graph.first_out);
	auto tail = invert_inverse_vector(first_out);
	auto head = move(routing_graph.head);
    auto geo_distance = move(routing_graph.geo_distance);
    auto travel_time = move(routing_graph.travel_time); // in secs
	latitude = move(routing_graph.latitude);
	longitude = move(routing_graph.longitude);

    // Build the index to quickly map latitudes and longitudes
    gpn = new GeoPositionToNode(latitude, longitude);
    //auto x = gpn->find_nearest_neighbor_within_radius(40.7121, -73.997, 200);
    //log_message("[TEST-C++] x.id: " + std::to_string(x.id));
    //log_message("[TEST-C++] x.distance: " + std::to_string(x.distance));

    // Save latitude, longitude vectors
    std::ofstream out_lat(basic_path + ".lat");
    std::ofstream out_lng(basic_path + ".lng");
    for (const auto &lat : latitude) out_lat << lat << " ";
    for (const auto &lng : longitude) out_lng << lng << " ";

    // Build the shortest path index
    log_message("Build CH");
    ch = new ContractionHierarchy(
                 ContractionHierarchy::build(first_out.size()-1, tail, head, travel_time, log_message));

    log_message("Save CH as a file");
    ch->save_file(basic_path + ".ch");

    log_message("Create CH query instance");
    ch_query = new ContractionHierarchyQuery(*ch);

    log_message("init_routingkit ends");

    return 0;
}

NearestNeighborhoodQueryResult find_nearest_node(float lat, float lng, float rad)
{
    // BUG (mwkwak, 190403): results are slightly different in certain conditions
    // case 1) direct use v.s. reuse lat, lng values
    // case 2) call in C++ example v.s. call in Go example
    NearestNeighborhoodQueryResult res = get_gpn()->find_nearest_neighbor_within_radius(lat, lng, rad);
    if(res.id == invalid_id) {
		std::cerr << "No node within " << rad << "m from the source position" << std::endl;
		return NearestNeighborhoodQueryResult{.id = 0, .distance = 0};
    }
    //std::cout << "find_nearest_node(), lat: " << lat << ", lng: " << lng << ", rad: " << rad << std::endl;
    //std::cout << "id : " << res.id << ", distance: " << res.distance << std::endl;
    return res;
}

ShortestPathQueryResult find_shortest_path(unsigned from_id, unsigned to_id)
{
    long long start_time = get_micro_time();
    ch_query->reset().add_source(from_id).add_target(to_id).run();
    long long end_time = get_micro_time();

    auto distance = ch_query->get_distance(); // time cost (in milliseconds). not actual 'distance'
    auto node_path = ch_query->get_node_path();
    auto arc_path = ch_query->get_arc_path();

    std::cout << "To get from "<< from_id << " to "<< to_id << " one needs " << distance << " milliseconds." << std::endl;
    std::cout << "This query was answered in " << (end_time - start_time) << " microseconds." << std::endl;
    std::cout << "Node path: ";
    for(auto x:node_path) std::cout << " " << x;
    std::cout << std::endl;

    std::cout << "Arc path: ";
    for(auto x:arc_path) std::cout << " " << x;
    std::cout << std::endl;

    return ShortestPathQueryResult{.total_distance = distance, .node_path = node_path, .arc_path = arc_path};
}


}
