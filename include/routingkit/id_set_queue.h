#ifndef ROUTING_KIT_ID_SET_QUEUE_H
#define ROUTING_KIT_ID_SET_QUEUE_H

#include <routingkit/constants.h>
#include <routingkit/min_max.h>
#include <vector>
#include <assert.h>

namespace RoutingKit{

class IDSetMinQueue{

private:
	static const unsigned root = 1;

	static unsigned parent(unsigned x){
		return x>>1;
	}

	static unsigned other_child(unsigned x){
		return x^1;
	}

	static unsigned is_larger_child(unsigned x){
		return x&1;
	}

	static unsigned is_smaller_child(unsigned x){
		return !is_larger_child(x);
	}

	static unsigned smaller_child(unsigned x){
		return x<<1;
	}

	static unsigned larger_child(unsigned x){
		return (x<<1)|1;
	}

	static unsigned smallest_two_power_no_smaller_than(unsigned x){
		unsigned y = 1;
		while(y < x){
			y <<= 1;
			assert(y != 0);
		}
		return y;
	}

public:
	IDSetMinQueue(){}

	explicit IDSetMinQueue(unsigned n):
		min_id(invalid_id),
		offset(smallest_two_power_no_smaller_than(n)),
		data(n+offset, false){}

	unsigned id_count() const {
		return data.size()-offset;
	}

	void push(unsigned id){
		assert(id < data.size()-offset && "id out of bounds");

		if(min_id == invalid_id)
			min_id = id;
		else		
			min_to(min_id, id);

		unsigned x = id + offset;

		if(data[x])
			return;

		for(;;)
		{
			assert(!data[x]);
			data[x] = true;

			if(x == root)
				break;

			x = parent(x);

			if(data[x])
				break;
		}
	}

	bool contains(unsigned id) const {
		assert(offset + id < data.size() && "id out of bounds");
		return data[offset + id];
	}

	bool empty() const {
		return min_id == invalid_id;
	}

	unsigned peek() const {
		return min_id;
	}

	unsigned pop() {
		assert(!empty());

		unsigned ret = min_id;

		unsigned x = min_id + offset;

		for(;;)
		{
			assert(data[x]);
			data[x] = false;

			if(x == root){
				min_id = invalid_id;
				return ret;
			}

			if(is_smaller_child(x)){
				if(data[other_child(x)])
					break;
			} else {
				assert(!data[other_child(x)]);
			}

			x = parent(x);
		}

		assert(is_smaller_child(x));
		x = other_child(x);
		assert(data[x]);
		for(;;){
			if(x >= offset)
				break;

			x = smaller_child(x);
			if(!data[x])
				x = other_child(x);
			assert(data[x]);
		}
		min_id = x - offset;
		
		return ret;
	}

	void clear(){
		while(!empty())
			pop();
	}

private:
	unsigned min_id;
	unsigned offset;
	
	std::vector<bool>data;
};

} // RoutingKit

#endif

