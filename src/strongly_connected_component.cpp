#include <routingkit/strongly_connected_component.h>
#include <routingkit/min_max.h>

namespace RoutingKit{

StronlyConnectedComponentsResult compute_strongly_connected_components(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head){
	const unsigned node_count = first_out.size()-1;

	std::vector<unsigned> dfs_stack(node_count);
	unsigned dfs_stack_top = 0;

	std::vector<unsigned> scc_stack(node_count);
	unsigned scc_stack_top = 0;

	std::vector<bool> in_scc_stack(node_count, false);

	std::vector<unsigned>next_out = first_out;

	constexpr unsigned invalid = (unsigned)-1;

	std::vector<unsigned> dfs_pos(node_count, invalid);
	unsigned next_preorder_id = 0;

	std::vector<unsigned> low_link(node_count);

	std::vector<bool> in_component(node_count, false);

	StronlyConnectedComponentsResult result = {std::vector<unsigned>(node_count), 0};

	for(unsigned r=0; r<node_count; ++r){
		if(!in_component[r]){
			dfs_stack[dfs_stack_top++] = r;
			while(dfs_stack_top != 0){
				unsigned x = dfs_stack[--dfs_stack_top];

				if(dfs_pos[x] == invalid){
					dfs_pos[x] = next_preorder_id;
					low_link[x] = next_preorder_id;
					++next_preorder_id;
					in_scc_stack[x] = true;
					scc_stack[scc_stack_top++] = x;
				}

				auto x_end = first_out[x+1];
				while(next_out[x] != x_end && dfs_pos[head[next_out[x]]] != invalid){
					if(in_scc_stack[head[next_out[x]]])
						min_to(low_link[x], low_link[head[next_out[x]]]);
					++next_out[x];
				}
				if(next_out[x] == x_end){
					if(dfs_pos[x] == low_link[x]){
						unsigned z;
						unsigned component_id = result.component_count++;
						do{
							z = scc_stack[--scc_stack_top];
							in_scc_stack[z] = false;
							result.component_of[z] = component_id;
							in_component[z] = true;
						}while(z != x);
					}
				}else if(dfs_pos[head[next_out[x]]] == invalid){
					dfs_stack[dfs_stack_top++] = x;
					dfs_stack[dfs_stack_top++] = head[next_out[x]];
				}
			}
			assert(scc_stack_top == 0);
		}
	}

	return result; // NVRO
}


std::vector<bool>compute_largest_strongly_connected_component(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head){
	const unsigned node_count = first_out.size()-1;
	std::vector<bool>result(node_count);
	if(node_count > 0){

		StronlyConnectedComponentsResult scc = compute_strongly_connected_components(first_out, head);

		std::vector<unsigned>component_size(scc.component_count, 0);
		for(unsigned x=0; x<node_count; ++x)
			++component_size[scc.component_of[x]];

		unsigned largest_component = 0;
		for(unsigned i=1; i<scc.component_count; ++i){
			if(component_size[largest_component] < component_size[i])
				largest_component = i;
		}

		for(unsigned x=0; x<node_count; ++x){
			result[x] = (scc.component_of[x] == largest_component);
		}
	}
	return result;
}


} // RoutingKit
