#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <routingkit/id_queue.h>
#include <routingkit/constants.h>
#include <routingkit/timestamp_flag.h>
#include <vector>

namespace RoutingKit{

template<class GetWeightFunc>
class Dijkstra{
public:
	Dijkstra(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head, GetWeightFunc get_weight):
		tentative_distance(first_out.size()-1),
		predecessor(first_out.size()-1),
		predecessor_arc(first_out.size()-1),
		was_popped(first_out.size()-1),
		queue(first_out.size()-1), 
		first_out(first_out), 
		head(head), 
		get_weight(std::move(get_weight)){}

	void clear(){
		queue.clear();
		was_popped.reset_all();
	}

	void add_source_node(unsigned id, unsigned departure_time = 0){
		tentative_distance[id] = departure_time;
		predecessor[id] = invalid_id;
		predecessor_arc[id] = invalid_id;
		queue.push({id, departure_time});
	}


	bool is_finished()const{
		return queue.empty();
	}

	IDKeyPair settle(){
		assert(!is_finished());

		auto p = queue.pop();
		tentative_distance[p.id] = p.key;
		was_popped.set(p.id);

		for(unsigned a=first_out[p.id]; a<first_out[p.id+1]; ++a){
			if(!was_popped.is_set(head[a])){
				unsigned w = get_weight(a, p.key);
				if(w < inf_weight){
					if(queue.contains_id(head[a])){
						if(queue.decrease_key({head[a], p.key + w})){
							predecessor[head[a]] = p.id;
							predecessor_arc[head[a]] = a;
						}
					} else {
						queue.push({head[a], p.key + w});
						predecessor[head[a]] = p.id;
						predecessor_arc[head[a]] = a;
					}
				}
			}
		}
		return p;
	}

	unsigned distance_to(unsigned x) const {
		if(was_popped.is_set(x))
			return tentative_distance[x];
		else
			return inf_weight;
	}

	std::vector<unsigned>path_to(unsigned x) const {
		std::vector<unsigned>path;
		while(predecessor[x] != invalid_id){
			path.push_back(x);
			x = predecessor[x];
		}
		path.push_back(x);
		std::reverse(path.begin(), path.end());
		return path;
	}

	std::vector<unsigned>arc_path_to(unsigned x) const {
		std::vector<unsigned>path;
		while(predecessor[x] != invalid_id){
			path.push_back(predecessor_arc[x]);
			x = predecessor[x];
		}
		std::reverse(path.begin(), path.end());
		return path;
	}

private:
	std::vector<unsigned>tentative_distance;
	std::vector<unsigned>predecessor;
	std::vector<unsigned>predecessor_arc;

	TimestampFlags was_popped;
	MinIDQueue queue;

	const std::vector<unsigned>&first_out;
	const std::vector<unsigned>&head;
	GetWeightFunc get_weight;
};

template<class GetWeightFunc>
Dijkstra<GetWeightFunc> make_dijkstra(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head, GetWeightFunc get_weight){
	return {first_out, head, std::move(get_weight)};
}

struct StaticGetWeight{
	explicit StaticGetWeight(const std::vector<unsigned>&weight):weight(&weight){}

	unsigned operator()(unsigned arc, unsigned departure_time)const{
		(void)departure_time;
		return (*weight)[arc];
	}

	const std::vector<unsigned>*weight;
};

inline
Dijkstra<StaticGetWeight> make_dijkstra(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head, const std::vector<unsigned>&weight){
	return {first_out, head, StaticGetWeight(weight)};
}

}

#endif
