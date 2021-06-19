#ifndef BIT_SELECT_H
#define BIT_SELECT_H

#include <stdint.h>

namespace RoutingKit{

// returns the 0-based offset of the (n+1)-th bit set in data

/**
 * Function that retrieves the bit with index n from a 64 bit word
 * 
 * @param word The word from which the bit will be retrieved
 * @param n The position of the bit retrieved
 */
uint32_t uint64_bit_select(uint64_t data, uint32_t n);


/**
 * Function that retrieves the bit with index n from a 512 bit block
 * 
 * @param block The word from which the bit will be retrieved
 * @param n The position of the bit retrieved
 */ 
uint32_t uint512_bit_select(const uint64_t*data, uint32_t n);

/**
 * Function that selects bits from a data
 * 
 * @param uint512_count The number of bits to be selected
 * @param uint512_rank The rank of the selected bits
 * @param data The actual data
 * @param n The position of the bits
 */ 
uint64_t bit_select(uint64_t uint512_count, const uint64_t*uint512_rank, const uint64_t*data, uint64_t n);

/**
 * Function that skips empty data
 * 
 * @param uint512_count The number of bits to be skipped
 * @param uint512_rank The rank of the bits
 */ 
uint64_t skip_empty_uint512(uint64_t&uint512_count, const uint64_t*&uint512_rank, const uint64_t*&data);

} // namespace RoutingKit

#endif
