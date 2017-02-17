#ifndef VERIFY_H
#define VERIFY_H

#include <vector>

namespace RoutingKit{

void check_if_graph_is_valid(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head);

void check_if_td_graph_is_valid(
	unsigned period,
	const std::vector<unsigned>&first_out, const std::vector<unsigned>&head,
	const std::vector<unsigned>&first_ipp_of_arc, const std::vector<unsigned>&ipp_departure_time, const std::vector<unsigned>&ipp_travel_time
);

void check_if_arc_ipp_are_valid(
	unsigned period,
	const std::vector<unsigned>&first_ipp_of_arc, const std::vector<unsigned>&ipp_departure_time, const std::vector<unsigned>&ipp_travel_time
);

void check_if_sst_queries_are_valid(
	unsigned period, unsigned node_count,
	const std::vector<unsigned>&source, const std::vector<unsigned>&source_time, const std::vector<unsigned>&target,
	const std::vector<unsigned>&rank
);

} // RoutingKit

#endif
