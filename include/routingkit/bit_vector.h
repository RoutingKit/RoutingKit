#ifndef ROUTING_KIT_BIT_VECTOR_H
#define ROUTING_KIT_BIT_VECTOR_H

#include <stdint.h>
#include <assert.h>
#include <utility>

/**
* RoutingKit is the namespace for the entire project
*/
namespace RoutingKit{


/**
 * Class BitVector contains all the information needed for a bit vector in this project
 */
class BitVector{
public:
	/**
	 * Unintialized is the structure that keeps uninitialized fields within the class
	 * It is used as a static field
	 */
	struct Uninitialized{};
	static constexpr Uninitialized uninitialized = {};

	/**
	 * Default Constructor
	 */
	BitVector();
	
	/**
	 * Constructor that takes the size
	 * 
	 * @param size Size of the BitVector object
	 */ 
	BitVector(uint64_t size, Uninitialized);
	
	/**
	 * Constructor that takes the size and an initial value
	 * 
	 * @param size Size of the BitVector object
	 * @param init_value Initial value of the BitVector object
	 */ 
	explicit BitVector(uint64_t size, bool init_value = false);
	
	/**
	 * Destructor
	 */
	~BitVector();
	
	/**
	 * Constructor that takes another const BitVector object
	 * 
	 * @param o BitVector object to be copied into new object 
	 */ 
	BitVector(const BitVector&);
	
	/**
	 * Constructor that takes another BitVector object
	 * 
	 * @param o BitVector object to be copied into new object, which is then turned into an invalid object
	 */ 
	BitVector(BitVector&&);
	
	/**
	 * Override of =
	 * 
	 * @param o BitVector object to be copied into new object
	 */ 
	BitVector&operator=(BitVector);
	
	/**
	 * Swap function
	 * 
	 * @param o BitVector object to be swaped with object
	 */ 
	void swap(BitVector&);
	
	bool empty()const { return size() == 0; }
	uint64_t size()const { return size_; }
	
	/**
	 * Resize function
	 * 
	 * @param new_size New size of the BitVector object
	 */ 
	void resize(uint64_t size, Uninitialized);
	
	
	/**
	 * Resize function
	 * 
	 * @param new_size New size of the BitVector object
	 * @param value The new value of the BitVector function
	 */ 	
	void resize(uint64_t size, bool init_value = false);

	/**
	 * Make large enough for function.
	 * Resizes the BitVector until is large enough for a give size
	 * 
	 * @param x The size for which the object should be large enough
	 */ 
	void make_large_enough_for(uint64_t x, Uninitialized);
	
	/**
	 * Make large enough for function.
	 * Resizes the BitVector until is large enough for a give size and gives it an initial value
	 * 
	 * @param x The size for which the object should be large enough
	 * @param init_value The initial value given 
	 */ 
	void make_large_enough_for(uint64_t x, bool init_value = false);

	/*
	 * @param x data to be checked 
	 * @return true if data is set, flase if not
	 */
	bool is_set(uint64_t x)const{
		assert(x < size_ && "argument out of bounds");
		uint64_t a = x/64;
	    uint64_t b = x%64;
		uint64_t d = data_[a];
		return d & (1ull << b);
	}

	/*
	 * Sets a data
	 * @param x data to be set
	 */
	void set(uint64_t x){
		assert(x < size_ && "argument out of bounds");
		uint64_t a = x/64;
	    uint64_t b = x%64;
		uint64_t d = data_[a];
		d |= (1ull << b);
		data_[a] = d;
	}
	
	/*
	 * Sets a data if a condition is fulfield
	 * @param x data to be set
	 * @param values condition
	 */
	void set_if(uint64_t x, bool value){
		assert(x < size_ && "argument out of bounds");
		uint64_t a = x/64;
	    uint64_t b = x%64;
		uint64_t d = data_[a];
		d |= ((uint64_t)value << b);
		data_[a] = d;
	}

	/*
	 * Sets a data
	 * @param x data to be set
	 */
	void set(uint64_t x, bool value){
		assert(x < size_ && "argument out of bounds");
		uint64_t a = x/64;
	    uint64_t b = x%64;
		uint64_t d = data_[a];
		d &= ~(1ull << b);
		d |= ((uint64_t)value << b);
		data_[a] = d;
	}
	
	/*
	 * Resets a data
	 * @param x data to be reset
	 */
	void reset(uint64_t x){
		assert(x < size_ && "argument out of bounds");
		uint64_t a = x/64;
	    uint64_t b = x%64;
		uint64_t d = data_[a];
		d &= ~(1ull << b);
		data_[a] = d;
	}
	
	/*
	 * Toggle a data
	 * @param x data to be toggled
	 */
	void toggle(uint64_t x){
		assert(x < size_ && "argument out of bounds");
		uint64_t a = x/64;
	    uint64_t b = x%64;
		uint64_t d = data_[a];
		d ^= (1ull << b);
		data_[a] = d;
	}

	/*
	 * Sets every field
	 */
	void set_all();
	void set_all(bool value);
	
	/*
	 * Resets every field
	 */
	void reset_all();

	/**
	 * @return true if all the bits are set, flase if not
	 */ 
	bool are_all_set()const;
	
	/**
	 * @return true if there us any bit set, flase if not
	 */ 
	bool is_any_set()const;

	/**
	 * Counts the population
	 */ 
	uint64_t population_count() const;
	
	/**
	 * Counts the true population
	 */ 
	uint64_t count_true() const {return population_count();}
	
	/**
	 * Counts the false population
	 */ 
	uint64_t count_false() const {return size()-population_count();}

	/**
	 * Override of |=
	 */ 
	BitVector&operator|=(const BitVector&);
	
	/**
	 * Override of ^=
	 */ 
	BitVector&operator^=(const BitVector&);
	
	/**
	 * Override of =
	 */ 
	BitVector&operator&=(const BitVector&);

	void inplace_not();
	
	/**
	 * Override of ~
	 */ 
	BitVector operator~() const { BitVector v = *this; v.inplace_not(); return v; }

	/**
	 * Override of ==
	 * 
	 * @param l BitVector object to be compared as lhs
	 * @param r BitVector object to be compared as rhs
	 */ 
	friend bool operator==(const BitVector&l, const BitVector&r);
	
	/**
	 * Override of <
	 * 
	 * @param l BitVector object to be compared as lhs
	 * @param r BitVector object to be compared as rhs
	 */ 
	friend bool operator<(const BitVector&l, const BitVector&r);

	/**
	 * Returns the private field data
	 */ 
	uint64_t*data(){return data_;}
	
	/**
	 * Returns const the private field data
	 */ 
	const uint64_t*data()const{return data_;}

	/**
	 * Returns the uint count
	 */ 
	uint64_t uint512_count()const{ return (size_+511) / 512; }

	/**
	 * Resets the paddign bits for a BitVector object
	 */ 
	void reset_all_padding_bits();
private:
	uint64_t*data_;
	uint64_t size_;
};

template<class F>
inline BitVector make_bit_vector(uint64_t size, const F&f){
	BitVector v(size, BitVector::uninitialized);
	for(uint64_t x=0; x<size; ++x)
		v.set(x, f(x));
	return v;
}

// I envy the day where C++ finally gets rid of this error prone boilerplate.

inline BitVector operator|(BitVector&&l, BitVector&&r) { l |= r; return std::move(l); }
inline BitVector operator|(BitVector&&l, const BitVector&r) { l |= r; return std::move(l); }
inline BitVector operator|(const BitVector&l, BitVector&&r) { r |= l; return std::move(r); }
inline BitVector operator|(const BitVector&l, const BitVector&r) { BitVector x = l; x |= r; return x; }

inline BitVector operator&(BitVector&&l, BitVector&&r) { l &= r; return std::move(l); }
inline BitVector operator&(BitVector&&l, const BitVector&r) { l &= r; return std::move(l); }
inline BitVector operator&(const BitVector&l, BitVector&&r) { r &= l; return std::move(r); }
inline BitVector operator&(const BitVector&l, const BitVector&r) { BitVector x = l; x &= r; return x; }

inline BitVector operator^(BitVector&&l, BitVector&&r) { l ^= r; return std::move(l); }
inline BitVector operator^(BitVector&&l, const BitVector&r) { l ^= r; return std::move(l); }
inline BitVector operator^(const BitVector&l, BitVector&&r) { r ^= l; return std::move(r); }
inline BitVector operator^(const BitVector&l, const BitVector&r) { BitVector x = l; x ^= r; return x; }

inline bool operator!=(const BitVector&l, const BitVector&r){ return !(l == r); }
inline bool operator>(const BitVector&l, const BitVector&r){ return r < l; }
inline bool operator<=(const BitVector&l, const BitVector&r){ return !(l > r); }
inline bool operator>=(const BitVector&l, const BitVector&r){ return !(l < r); }

} // namespace RoutingKit

#endif
