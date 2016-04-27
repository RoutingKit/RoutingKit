#ifndef BIT_SELECT_H
#define BIT_SELECT_H

#include <stdint.h>


// The functions in this header are not intended to be used directly by the 
// user. They are the building blocks of id_mapper.h wrapped in a public 
// interface to be accessible by unit tests.

// returns the 0-based offset of the (n+1)-th bit set in data 

namespace RoutingKit{

uint32_t uint64_bit_select(uint64_t data, uint32_t n);
uint32_t uint512_bit_select(const uint64_t*data, uint32_t n);
uint64_t bit_select(uint64_t uint512_count, const uint64_t*uint512_rank, const uint64_t*data, uint64_t n);
uint64_t skip_empty_uint512(uint64_t&uint512_count, const uint64_t*&uint512_rank, const uint64_t*&data);

} // namespace RoutingKit

#endif
