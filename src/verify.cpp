#include "verify.h"

#include <algorithm>
#include <stdexcept>

namespace RoutingKit{

void check_if_graph_is_valid(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head){
	if(first_out.front() != 0)
		throw std::runtime_error("first_out[0] must be 0");
	if(first_out.back() != head.size())
		throw std::runtime_error("first_out.back() must be head.size()");

	unsigned node_count = first_out.size()-1;

	if(!std::is_sorted(first_out.begin(), first_out.end()))
		throw std::runtime_error("first_out must be sorted");

	for(auto&x:head)
		if(x >= node_count)
			throw std::runtime_error("all heads must be at most node_count");
}

void check_if_arc_ipp_are_valid(
	unsigned period,
	const std::vector<unsigned>&first_ipp_of_arc, const std::vector<unsigned>&ipp_departure_time, const std::vector<unsigned>&ipp_travel_time
){
	if(first_ipp_of_arc.front() != 0)
		throw std::runtime_error("first_ipp_of_arc[0] must be 0");
	if(first_ipp_of_arc.back() != ipp_departure_time.size())
		throw std::runtime_error("first_ipp_of_arc.back() must be ipp_departure_time.size()");

	if(!std::is_sorted(first_ipp_of_arc.begin(), first_ipp_of_arc.end()))
		throw std::runtime_error("first_ipp_of_arc must be sorted");

	for(unsigned i=1; i<first_ipp_of_arc.size(); ++i){
		if(first_ipp_of_arc[i-1] == first_ipp_of_arc[i])
			throw std::runtime_error("every arc must have at least one ipp");
	}		

	if(ipp_travel_time.size() != ipp_departure_time.size())
		throw std::runtime_error("ipp_travel_time.size() must be ipp_departure_time.size()");

	for(unsigned a=0; a<first_ipp_of_arc.size()-1; ++a)
		if(!std::is_sorted(ipp_departure_time.begin() + first_ipp_of_arc[a], ipp_departure_time.begin() + first_ipp_of_arc[a+1]))
			throw std::runtime_error("ipp_departure_time of every arc must be sorted");

	for(auto x:ipp_departure_time)
		if(x >= period)
			throw std::runtime_error("ipp_departure_time must be smaller than the period");
}

void check_if_td_graph_is_valid(
	unsigned period,
	const std::vector<unsigned>&first_out, const std::vector<unsigned>&head,
	const std::vector<unsigned>&first_ipp_of_arc, const std::vector<unsigned>&ipp_departure_time, const std::vector<unsigned>&ipp_travel_time
)
{
	check_if_graph_is_valid(first_out, head);
	if(head.size() != first_ipp_of_arc.size()-1)
		throw std::runtime_error("head.size() must be first_ipp_of_arc.size()-1");
	check_if_arc_ipp_are_valid(period, first_ipp_of_arc, ipp_departure_time, ipp_travel_time);
	
}




void check_if_sst_queries_are_valid(
	unsigned period, unsigned node_count,
	const std::vector<unsigned>&source, const std::vector<unsigned>&source_time, const std::vector<unsigned>&target,
	const std::vector<unsigned>&rank
)
{
	for(auto x:source)
		if(x >= node_count)
			throw std::runtime_error("source node is out of range");
	for(auto x:target)
		if(x >= node_count)
			throw std::runtime_error("target node is out of range");
	for(auto x:source_time)
		if(x >= period)
			throw std::runtime_error("source time is out of range");


	if(source_time.size() != source.size())
		throw std::runtime_error("source_time has not the same size as source");
	if(target.size() != source.size())
		throw std::runtime_error("target has not the same size as source");
	if(rank.size() != source.size())
		throw std::runtime_error("rank has not the same size as source");
}

} // RoutingKit

