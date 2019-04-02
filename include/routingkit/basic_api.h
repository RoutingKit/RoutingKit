#ifndef ROUTING_KIT_BASIC_API_H
#define ROUTING_KIT_BASIC_API_H

#include <routingkit/osm_graph_builder.h>
#include <routingkit/osm_profile.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/contraction_hierarchy.h>

#include <iostream>
#include <string>

namespace RoutingKit {

// Load OSM data (at once) and build contraction hierarchies
int init_routingkit_ch(std::string pbf_file);//, bool print_log);

}

#endif // ROUTING_KIT_BASIC_API_H
