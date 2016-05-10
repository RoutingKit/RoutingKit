#include "protobuf.h"

namespace RoutingKit{

uint64_t decode_varint_as_uint64_and_advance_first_parameter(const char*&begin, const char*end){
	if(begin == end)
			throw std::runtime_error("Protobuf message is corrupt because varint was expected at the end of the message.");
	
	uint64_t n = 0;
	uint64_t shift = 0;

	uint8_t high_bit = 1u<<7;
	uint8_t mask = ~high_bit;

	while((*begin & high_bit) != 0){
		if(shift == 7*9 && ((uint8_t)*begin) != (1|high_bit))
			throw std::runtime_error("Cannot decode varint because the value does not fit into 64 bit.");
		n |= ((uint64_t)(((uint8_t)*begin) & mask))<<shift;
		shift += 7;
		++begin;
		if(begin == end)
			throw std::runtime_error("Protobuf message is corrupt because varint was not terminated before the end of the message.");
	}
	n |= ((uint64_t)(((uint8_t)*begin) & mask)) << shift;
	++begin;

	return n;
}

int64_t zigzag_convert_uint64_to_int64(uint64_t x){
	if((x & 1))
		return -static_cast<int64_t>(x>>1)-1;
	else
		return static_cast<int64_t>(x>>1);
}

}
