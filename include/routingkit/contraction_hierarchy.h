#ifndef ROUTING_KIT_CONTRACTION_HIERARCHY_H
#define ROUTING_KIT_CONTRACTION_HIERARCHY_H

#include <routingkit/id_queue.h>
#include <routingkit/timestamp_flag.h>
#include <routingkit/bit_vector.h>
#include <routingkit/permutation.h>

#include <vector>

namespace RoutingKit{

class ContractionHierarchy{
public:
	static const unsigned default_max_pop_count = 500;

	static ContractionHierarchy build(
		unsigned node_count, std::vector<unsigned>tail, std::vector<unsigned>head, std::vector<unsigned>weight, 
		const std::function<void(std::string)>&log_message = std::function<void(std::string)>(), unsigned max_pop_count = default_max_pop_count
	);

	static ContractionHierarchy build_given_rank(
		std::vector<unsigned>rank,
		std::vector<unsigned>tail, std::vector<unsigned>head, std::vector<unsigned>weight, 
		const std::function<void(std::string)>&log_message = std::function<void(std::string)>(), unsigned max_pop_count = default_max_pop_count
	);

	static ContractionHierarchy build_given_order(
		std::vector<unsigned>order,
		std::vector<unsigned>tail, std::vector<unsigned>head, std::vector<unsigned>weight, 
		const std::function<void(std::string)>&log_message = std::function<void(std::string)>(), unsigned max_pop_count = default_max_pop_count
	);

	static ContractionHierarchy read(std::function<void(char*, unsigned long long)>data_source);
	static ContractionHierarchy read(std::function<void(char*, unsigned long long)>data_source, unsigned long long file_size);
	static ContractionHierarchy read(std::istream&in);
	static ContractionHierarchy read(std::istream&in, unsigned long long file_size);
	static ContractionHierarchy load_file(const std::string&file_name);

	void write(std::function<void(const char*, unsigned long long)>data_sink);
	void write(std::ostream&out);
	void save_file(const std::string&file_name);

	unsigned node_count()const{
		return rank.size();
	}

	struct Side{
		std::vector<unsigned>first_out;
		std::vector<unsigned>head;
		std::vector<unsigned>weight;

		BitVector is_shortcut_an_original_arc;
		std::vector<unsigned>shortcut_first_arc;
		std::vector<unsigned>shortcut_second_arc;
	};

	std::vector<unsigned>rank, order;
	Side forward, backward;
};

void check_contraction_hierarchy_for_errors(const ContractionHierarchy&ch);

class ContractionHierarchyQuery{
public:
	ContractionHierarchyQuery():ch(0){}
	explicit ContractionHierarchyQuery(const ContractionHierarchy&ch);

	ContractionHierarchyQuery&reset();
	ContractionHierarchyQuery&reset(const ContractionHierarchy&ch);

	ContractionHierarchyQuery&add_source(unsigned s, unsigned dist_to_s = 0);
	ContractionHierarchyQuery&add_target(unsigned t, unsigned dist_to_t = 0);

	ContractionHierarchyQuery&run();

	unsigned get_used_source();
	unsigned get_used_target();

	unsigned get_distance();
	std::vector<unsigned>get_node_path();
	std::vector<unsigned>get_arc_path();

	ContractionHierarchyQuery& reset_source();
	ContractionHierarchyQuery& pin_targets(const std::vector<unsigned>&);
	ContractionHierarchyQuery& run_to_pinned_targets();

	ContractionHierarchyQuery& get_distances_to_targets(unsigned*dist);
	std::vector<unsigned> get_distances_to_targets();
	
	ContractionHierarchyQuery& reset_target();
	ContractionHierarchyQuery& pin_sources(const std::vector<unsigned>&);
	ContractionHierarchyQuery& run_to_pinned_sources();

	ContractionHierarchyQuery& get_distances_to_sources(unsigned*dist);
	std::vector<unsigned> get_distances_to_sources();
		
//private:
	const ContractionHierarchy*ch;

	TimestampFlags was_forward_pushed, was_backward_pushed;
	MinIDQueue forward_queue, backward_queue;
	std::vector<unsigned>forward_tentative_distance, backward_tentative_distance;
	std::vector<unsigned>forward_predecessor_node, backward_predecessor_node;
	std::vector<unsigned>forward_predecessor_arc, backward_predecessor_arc;
	unsigned shortest_path_meeting_node;
	unsigned needed_for_one_to_many;
	unsigned state;
};

} // namespace RoutingKit

#endif

