#ifndef ROUTING_KIT_ID_MAPPER_H
#define ROUTING_KIT_ID_MAPPER_H

#include <routingkit/constants.h>

#include <stdint.h>
#include <assert.h>
#include <stdexcept>
#include <vector>
#include <algorithm>

namespace RoutingKit{

class LocalIDMapper{
public:
	LocalIDMapper():bit_count_(0){}
	template<class Vec>
	explicit LocalIDMapper(const Vec&vec):
		LocalIDMapper(vec.size(), vec.data()){}
	LocalIDMapper(uint64_t bit_count_, const uint64_t*bits_);

	uint64_t global_id_count()const{
		return bit_count_;
	}

	uint64_t local_id_count()const{ 
		if(!rank_.empty())
			return rank_.back(); 
		else
			return 0;
	}

	bool is_global_id_mapped(uint64_t global_id) const{
		assert(global_id < global_id_count());
		return (bits_[global_id / 64] & (1ull << (global_id % 64))) != 0;
	}

	uint64_t to_local(uint64_t global_id) const;
	uint64_t to_local(uint64_t global_id, uint64_t invalid) const;

	uint64_t memory_overhead_in_bits()const{return rank_.size()*64;}
protected:
	const uint64_t*bits_;
	uint64_t bit_count_;
	std::vector<uint64_t>rank_;
};

class IDMapper : public LocalIDMapper{
public:
	IDMapper(){}
	template<class Vec>
	explicit IDMapper(const Vec&vec):
		IDMapper(vec.size(), vec.data()){}
	IDMapper(uint64_t bit_count_, const uint64_t*bits_);

	uint64_t to_global(uint64_t local_id) const;

	uint64_t memory_overhead_in_bits()const{return LocalIDMapper::memory_overhead_in_bits() + select_.size()*64;}
private:
	std::vector<uint64_t>select_;
};

class GlobalToLocalIDMapper{
public:
	GlobalToLocalIDMapper():global_id_count_(0){}
	
	explicit GlobalToLocalIDMapper(uint64_t global_id_count){
		set_global_id_count(global_id_count);
	}

	GlobalToLocalIDMapper(uint64_t global_id_count, bool value){
		set_global_id_count(global_id_count);
		if(value)
			std::fill(flag_.begin(), flag_.end(), ~0ull);
		else
			std::fill(flag_.begin(), flag_.end(), 0ull);
	}

	void add_global_id(uint64_t global_id){
		assert(global_id < global_id_count_);
		flag_[global_id / 64ull] |= (1ull<<(global_id%64ull));
	}
	
	void remove_global_id(uint64_t global_id){
		assert(global_id < global_id_count_);
		flag_[global_id / 64ull] &= ~(1ull<<(global_id%64ull));
	}

	bool contains_global_id(uint64_t global_id) const {
		if(global_id < global_id_count_)
			return flag_[global_id / 64ull] & (1ull<<(global_id%64ull));
		else
			return false;
	}

	void set_global_id_count(uint64_t new_global_id_count){
		global_id_count_ = new_global_id_count;
		flag_.resize(((new_global_id_count/64ull + 7ull)/8ull) * 8ull, 0);
		flag_.shrink_to_fit();
	}

	void make_global_id_count_big_enough_for(uint64_t global_id){
		if(global_id >= global_id_count_) {
			uint64_t n = 1;
			while(n <= global_id) {
				n <<= 1;
			}
			set_global_id_count(n);
		}
		assert(global_id < global_id_count_);
	}

	uint64_t global_id_count() const {
		return global_id_count_;
	}

	uint32_t local_id_count() const {
		assert(!rank_.empty() && "must call compute_mapping first");
		return rank_.back();
	}

	void compute_mapping(){
		{		
			rank_.resize(flag_.size()/8+1);
			rank_.shrink_to_fit();

			uint64_t i = 0;
			uint32_t s = 0;
			for(uint64_t j = 0; j<rank_.size()-1; ++j){
				rank_[j] = s;
				uint32_t 
					s0 = __builtin_popcountll(flag_[i+0]),
					s1 = __builtin_popcountll(flag_[i+1]),
					s2 = __builtin_popcountll(flag_[i+2]),
					s3 = __builtin_popcountll(flag_[i+3]),
					s4 = __builtin_popcountll(flag_[i+4]),
					s5 = __builtin_popcountll(flag_[i+5]),
					s6 = __builtin_popcountll(flag_[i+6]),
					s7 = __builtin_popcountll(flag_[i+7]);

				i += 8;

				s0 += s4; s1 += s5; s2 += s6; s3 += s7;

				s0 += s2; s1 += s3;

				s0 += s1;
		
				s += s0;

				if(s < rank_[j])
					throw std::runtime_error("local ID range must have fewer than 2^32 elements");
			}
			rank_.back() = s;
		}
		{
			uint32_t select_query = 0, select_value = 0;
			select_.resize((local_id_count() + ((1<<select_bits)-1)) / (1<<select_bits));
			for(uint64_t block=0; block < rank_.size()-1; ++block){
				while(rank_[block] <= select_value && select_value < rank_[block+1]){
					select_[select_query] = block;
					++select_query;
					select_value = select_query << select_bits;
				}
			}
		}
	}

	uint32_t operator[](uint64_t global_id) const {
		return global_to_local(global_id);
	}

	uint32_t global_to_local(uint64_t global_id) const{
		assert(!rank_.empty() && "must call compute_mapping first");
		if(global_id > global_id_count_)
			return invalid_id;

		std::uint8_t in_word_offset = global_id % 64;
		uint64_t word_index = global_id / 64;

		if(__builtin_expect( (flag_[word_index] & (1ull<<(in_word_offset))) == 0, true)){
			return invalid_id;
		}

		uint64_t block_index = global_id / 512; 

		uint32_t local_id = rank_[block_index];

		for(uint64_t i=block_index*8; i<word_index; ++i)
			local_id += __builtin_popcountll(flag_[i]);

		local_id += __builtin_popcountll(flag_[word_index] & ((1ull<<in_word_offset)-1));

		assert(local_id < local_id_count());

		return local_id;
	}

	uint64_t local_to_global(uint32_t local_id) const{
		assert(local_id < local_id_count());

		unsigned global_id = scan_select(select_[local_id >> select_bits], local_id);

		assert(contains_global_id(global_id));
		assert(global_to_local(global_id) == local_id);

		return global_id;
	}

	uint64_t memory_consumption() const {
		return select_.capacity() * 8 + flag_.capacity() * 8 + rank_.capacity() * 4 + 8 + 2*sizeof(std::vector<uint64_t>) + sizeof(std::vector<uint32_t>);
	}

//private:

	static const int select_bits = 9;

	uint64_t scan_to_next_non_empty_block(uint64_t start_block)const{
		uint64_t current_block = start_block;
		uint32_t ones_until_current_block_starts = rank_[current_block];

		if(__builtin_expect(rank_[current_block+1] != ones_until_current_block_starts, false)){
			const uint64_t block_count = rank_.size()-1;
			while(rank_[current_block+1] == ones_until_current_block_starts){
				uint64_t step = 1;
				while(current_block+step < block_count && rank_[current_block+step] == ones_until_current_block_starts){
					step <<= 1;
				}
				step >>= 1;
				current_block += step;
				ones_until_current_block_starts = rank_[current_block];
			}
		}

		for(uint64_t i=start_block*8; i<current_block*8; ++i){
			assert(flag_[i] == 0);
		}

		return current_block;		
	}

	// returns the 0-based offset of the (n+1)-th bit set in word
	static uint32_t in_word_select(uint64_t word, uint32_t n) {
		assert(n < __builtin_popcountll(word) && "n is out of bounds");

		uint32_t r = 0;
		assert(n < 64);
		{
			uint32_t lower_ones = __builtin_popcount(static_cast<uint32_t>(word));
			if(lower_ones <= n){
				word >>= 32;
				n -= lower_ones;
				r += 32;
			}
		}
		assert(n < 32);
		{
			uint32_t lower_ones = __builtin_popcount(static_cast<uint32_t>(word)&0xFFFF);
			if(lower_ones <= n){
				word >>= 16;
				n -= lower_ones;
				r += 16;
			}
		}
		assert(n < 16);
		{
			uint32_t lower_ones = __builtin_popcount(static_cast<uint32_t>(word)&0xFF);
			if(lower_ones <= n){
				word >>= 8;
				n -= lower_ones;
				r += 8;
			}
		}	
		assert(n < 8);
		while(n > 0){
			// reset lowest bit
			word &= word - 1;
			--n;
		}
		// __builtin_ffsl returns the 1-based offset of lowest bit, or 0 if no bit is set
		uint32_t x = __builtin_ffsll(word);
		assert(x != 0);
		return r+x-1;
	}

	// returns the 0-based offset of the (n+1)-th bit set in block 
	static uint32_t in_block_select(const uint64_t*block, uint32_t n) {
		uint32_t r = 0;
		for(;;){
			uint8_t lower_ones = __builtin_popcountll(*block);
			if(lower_ones <= n){
				++block;
				n -= lower_ones;
				r += 64;
			}else{
				return r+in_word_select(*block, n);
			}
		}
	}

	// returns the 0-based offset of the (n+1)-th bit set in the whole array
	// search starts at block b. 
	// The (n+1)-th bit must be in the b-th block or in a later block.
	uint32_t scan_select(uint64_t b, uint32_t n) const {
		assert(n >= rank_[b] && "b is too large");
		while(n >= rank_[b+1])
			b = scan_to_next_non_empty_block(b+1);
		return in_block_select(&flag_[8*b], n - rank_[b])+b*512;
	}

	// flag_ is a large bit-array. It has global_id_count_ rounded up to the next higher boundary of 512 bits.
	// flag_ is logically paritioned into blocks of 512 bits (i.e. 8 uint64). Every block has an ID.
	// rank_[i] contains the number of ones over all blocks with an ID strictly smaller than i.
	// rank_[0] is always 0
	// rank_[chunk_count] contains the total number of ones
	// next_non_empty_[i] contains for every chunck the ID of the next chunk that is not empty (empty meaning that all bits are zero).

	uint64_t global_id_count_;
	std::vector<uint64_t>flag_;
	std::vector<uint32_t>rank_;
	std::vector<uint64_t>select_;
};

} // RoutingKit

#endif
