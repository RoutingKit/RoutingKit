#ifndef ROUTING_KIT_OSM_PROFILE_HPP
#define ROUTING_KIT_OSM_PROFILE_HPP

#include <routingkit/osm_graph_builder.h>
#include <routingkit/tag_map.h>

#include <functional>
#include <string>

namespace RoutingKit{

bool is_osm_way_used_by_pedestrians(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = [](const std::string&){});
bool is_osm_way_used_by_cars(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = [](const std::string&){});
OSMWayDirectionCategory get_osm_car_direction_category(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = [](const std::string&){});
unsigned get_osm_way_speed(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = [](const std::string&){});
std::string get_osm_way_name(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = [](const std::string&){});

} // RoutingKit

#endif

