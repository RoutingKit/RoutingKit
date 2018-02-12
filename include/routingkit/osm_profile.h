#ifndef ROUTING_KIT_OSM_PROFILE_HPP
#define ROUTING_KIT_OSM_PROFILE_HPP

#include <routingkit/osm_graph_builder.h>
#include <routingkit/tag_map.h>
#include <routingkit/osm_decoder.h>

#include <functional>
#include <string>

namespace RoutingKit{

bool is_osm_way_used_by_cars(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
// Returns either
//   - speed in km/h
//   - absolute seconds needed to traverse way
// If absolute seconds are returned the result is marked with ABSOLUTE_TIME flag.
const unsigned ABSOLUTE_TIME = 0x80000000;
unsigned get_osm_way_speed(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
std::string get_osm_way_name(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
OSMWayDirectionCategory get_osm_car_direction_category(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
void decode_osm_car_turn_restrictions(uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags, std::function<void(OSMTurnRestriction)>on_new_turn_restriction, std::function<void(const std::string&)>log_message = nullptr);

bool is_osm_way_used_by_bicycles(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
unsigned char get_osm_way_bicycle_comfort_level(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
OSMWayDirectionCategory get_osm_bicycle_direction_category(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
unsigned char get_min_bicycle_comfort_level();
unsigned char get_max_bicycle_comfort_level();

bool is_osm_way_used_by_pedestrians(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
} // RoutingKit

#endif

