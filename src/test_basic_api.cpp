#include <routingkit/basic_api.h>

int main(int argc, char** argv) {
    RoutingKit::init_routingkit("../data/new-york-latest.osm.pbf", true, true);
    auto from = RoutingKit::find_nearest_node(40.7612, -73.997, 200);
    std::cout << from.id << " // " << from.distance << std::endl;

    auto to = RoutingKit::find_nearest_node(40.7742, -73.9897, 200);
    std::cout << to.id << " // " << to.distance << std::endl;

    auto res = RoutingKit::find_shortest_path(39873, 571729);
    std::cout << res.total_distance << std::endl << std::endl;

    for (const auto &n : res.node_path)
        std::cout << n << " ";
    std::cout << std::endl << std::endl;
    
    for (const auto &a : res.arc_path)
        std::cout << a << " ";
    std::cout << std::endl << std::endl;
}
