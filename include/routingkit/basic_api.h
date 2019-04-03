#ifndef ROUTING_KIT_BASIC_API_H
#define ROUTING_KIT_BASIC_API_H

#include <routingkit/osm_simple.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/geo_position_to_node.h>
#include <routingkit/timer.h>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>

namespace RoutingKit {

struct ShortestPathQueryResult{
    unsigned total_distance;
    std::vector<unsigned> node_path;
    std::vector<unsigned> arc_path;
};

extern ContractionHierarchy* ch;
extern GeoPositionToNode* gpn;
extern ContractionHierarchyQuery* ch_query;

ContractionHierarchy* get_ch();
GeoPositionToNode* get_gpn();
ContractionHierarchyQuery* get_ch_query();

// Load OSM data (at once) and do some preprocessing (e.g., build CH and CH query)
int init_routingkit(std::string pbf_file, bool cold_start = false, bool print_log = false);
NearestNeighborhoodQueryResult find_nearest_node(float lat, float lng, float rad);
ShortestPathQueryResult find_shortest_path(unsigned from_id, unsigned to_id);
}

#endif // ROUTING_KIT_BASIC_API_H