#ifndef ROUTING_KIT_STRONGLY_CONNECTED_COMPONENT_H
#define ROUTING_KIT_STRONGLY_CONNECTED_COMPONENT_H

#include <vector>

namespace RoutingKit{

struct StronlyConnectedComponentsResult{
	std::vector<unsigned>component_of;
	unsigned component_count;
};

StronlyConnectedComponentsResult compute_strongly_connected_components(
	const std::vector<unsigned>&first_out, const std::vector<unsigned>&head
);

//! Computes a largest strongly connected component and returns a vector that is
//! true for every node in this component.
std::vector<bool>compute_largest_strongly_connected_component(
	const std::vector<unsigned>&first_out, const std::vector<unsigned>&head
);

} // RoutingKit

#endif
