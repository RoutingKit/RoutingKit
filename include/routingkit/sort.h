#ifndef ROUTING_KIT_SORT_H
#define ROUTING_KIT_SORT_H

#include <routingkit/permutation.h>

#include <vector>
#include <assert.h>
#include <algorithm>
#include <functional>

namespace RoutingKit{

//
// A sort permutation p of a vector v is a permutation such that 
// v[p[0]] <= v[p[1]] <= v[p[2]] <= ... . Applying a sort permutation p of v to 
// v yields a sorted vector.
//
// The functions in this header follow the following naming sheme:
//
//   1) compute_[inverse_][stable_]sort_using_[(key|less|comparator](vector, ...order...)
//   2) [stable_]sort_using_[key|less|comparator](vector, ...order...)
//   3) is_sorted_using_[key|less|comparator](vector, ...order...)
//
//
// The functionality is explained as following:
//
//   * The functions of category 1 compute sort permutations but do not actually
//     sort any array. The sorting is done by applying the resulting permutation
//     to one or more vectors.
//   * The functions of category 2 copy their argument vector, sort the copy and
//     return it. If an rvalue vector is given as parameter then the copy is 
//     omitted.
//   * The functions of category 3 sort their argument array inplace.
//   * The functions of category 4 do not sort anything but check whether their
//     argument array is sorted.
//   * "inverse" indicates that not a sort permutation but its inverse is 
//     computed. In combination with the _using_key function it is slightly 
//     faster to compute an inverse sort permutation and then to use 
//     apply_inverse_permutation then to compute a sort permutation and to use
//     apply_permutation.
//   * "stable" indicates that the relative order of values that compare equally
//     is not changed.
//   * "key" indicates that elements are sorted using bucket sort. All functions
//     take a (key_count, get_key) pair of parameters to specify the order. 
//     get_key is a function  object that must map every element onto a value in
//     [0,key_count).  get_key is assumed to have a quick execution and is 
//     called several times per sort operation. However, get_key is never 
//     copied.
//   * "comparator" indicates that an STL like comparator is used and needs to
//     be passed to every function.
//   * "less" indicates that < is used for comparision. No order paramters need
//     to be passed to the functions.
//
// TODO: Do not use bucket sort in _using_key function if there are significantly
//       less elements than keys.
//


namespace detail{
	template<class T, class K>
	std::vector<unsigned>compute_key_pos(const std::vector<T>&v, unsigned key_count, const K&get_key){
		std::vector<unsigned>key_pos(key_count, 0);
		for(unsigned i=0; i<v.size(); ++i){
			unsigned k = get_key(v[i]);
			assert(k <= key_count && "key is too large");
			++key_pos[k];
		}
		unsigned sum = 0;
		for(unsigned i=0; i<key_count; ++i){
			unsigned tmp = sum + key_pos[i];
			key_pos[i] = sum;
			sum = tmp;
		}
		return key_pos; // NVRO
	}
}

template<class T, class K>
std::vector<unsigned> compute_stable_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	std::vector<unsigned>key_pos = detail::compute_key_pos(v, key_count, get_key);
	std::vector<unsigned>p(v.size());
	for(unsigned i=0; i<v.size(); ++i){
		unsigned k = get_key(v[i]);
		assert(k <= key_count && "key is too large");
		p[key_pos[k]] = i;
		++key_pos[k];
	}
	return p; // NVRO
}

template<class T, class K>
std::vector<unsigned> compute_inverse_stable_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	std::vector<unsigned>key_pos = detail::compute_key_pos(v, key_count, get_key);
	std::vector<unsigned>p(v.size());
	for(unsigned i=0; i<v.size(); ++i){
		unsigned k = get_key(v[i]);
		assert(k <= key_count && "key is too large");
		p[i] = key_pos[k];
		++key_pos[k];
	}
	return p; // NVRO
}

template<class T, class K>
std::vector<T>stable_sort_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	std::vector<unsigned>key_pos = detail::compute_key_pos(v, key_count, get_key);
	std::vector<T>r(v.size());

	for(unsigned i=0; i<v.size(); ++i){
		unsigned k = get_key(v[i]);
		assert(k <= key_count && "key is too large");
		r[key_pos[k]] = v[i];
		++key_pos[k];
	}

	return r; // NVRO
}

template<class T, class K>
std::vector<T>stable_sort_using_key(std::vector<T>&&v, unsigned key_count, const K&get_key){
	std::vector<unsigned>key_pos = detail::compute_key_pos(v, key_count, get_key);
	std::vector<T>r(v.size());
	for(unsigned i=0; i<v.size(); ++i){
		unsigned k = get_key(v[i]);
		assert(k <= key_count && "key is too large");
		r[key_pos[k]] = std::move(v[i]);
		++key_pos[k];
	}
	return r; // NVRO
}

template<class T, class K>
std::vector<T>sort_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	return stable_sort_using_key(v, key_count, get_key); // NVRO
}

template<class T, class K>
std::vector<T>sort_using_key(std::vector<T>&&v, unsigned key_count, const K&get_key){
	return stable_sort_using_key(std::move(v), key_count, get_key); // NVRO
}

template<class T, class K>
std::vector<unsigned> compute_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	return compute_stable_sort_permutation_using_key(v, key_count, get_key);
}

template<class T, class K>
std::vector<unsigned> compute_inverse_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	return compute_inverse_stable_sort_permutation_using_key(v, key_count, get_key);
}

template<class T, class K>
bool is_sorted_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	for(unsigned i=1; i<v.size(); ++i)
		if(get_key(v[i]) < get_key(v[i-1]))
			return false;
	return true;
}





template<class T, class C>
std::vector<unsigned> compute_stable_sort_permutation_using_comparator(const std::vector<T>&v, const C&is_less){
	std::vector<unsigned>p = identity_permutation(v.size());
	std::stable_sort(p.begin(), p.end(), [&](unsigned l, unsigned r){return is_less(v[l], v[r]);});
	return p; // NVRO
}

template<class T, class C>
std::vector<T>stable_sort_using_comparator(const std::vector<T>&v, const C&is_less){
	std::vector<T>r = v;
	std::stable_sort(r.begin(), r.end(), is_less);
	return r; // NVRO
}

template<class T, class C>
std::vector<T>stable_sort_using_comparator(std::vector<T>&&v, const C&is_less){
	std::stable_sort(v.begin(), v.end(), is_less);
	return std::move(v); // NVRO
}

template<class T, class C>
std::vector<unsigned> compute_sort_permutation_using_comparator(const std::vector<T>&v, const C&is_less){
	std::vector<unsigned>p = identity_permutation(v.size());
	std::sort(p.begin(), p.end(), [&](unsigned l, unsigned r){return is_less(v[l], v[r]);});
	return p; // NVRO
}

template<class T, class C>
std::vector<T>sort_using_comparator(const std::vector<T>&v, const C&is_less){
	std::vector<T>r = v;
	std::sort(r.begin(), r.end(), is_less);
	return r; // NVRO
}

template<class T, class C>
std::vector<T>sort_using_comparator(std::vector<T>&&v, const C&is_less){
	std::sort(v.begin(), v.end(), is_less);
	return std::move(v); // NVRO
}

template<class T>
std::vector<unsigned> compute_sort_permutation_using_less(const std::vector<T>&v){
	return compute_sort_permutation_using_comparator(v, [](const T&l, const T&r){return l < r;}); 
}

template<class T>
std::vector<unsigned> compute_stable_sort_permutation_using_less(const std::vector<T>&v){
	return compute_stable_sort_permutation_using_comparator(v, [](const T&l, const T&r){return l < r;}); 
}

template<class T>
std::vector<T>stable_sort_using_less(const std::vector<T>&v){
	return stable_sort_using_comparator(v, [](const T&l, const T&r){return l < r;});
}

template<class T>
std::vector<T>stable_sort_using_less(std::vector<T>&&v){
	return stable_sort_using_comparator(std::move(v), [](const T&l, const T&r){return l < r;});
}

template<class T>
std::vector<T>sort_using_less(const std::vector<T>&v){
	return sort_using_comparator(v, [](const T&l, const T&r){return l < r;});
}

template<class T>
std::vector<T>sort_using_less(std::vector<T>&&v){
	return sort_using_comparator(std::move(v), [](const T&l, const T&r){return l < r;});
}

template<class T, class C>
bool is_sorted_using_comparator(const std::vector<T>&v, const C&is_less){
	for(unsigned i=1; i<v.size(); ++i)
		if(is_less(v[i], v[i-1]))
			return false;
	return true;
}

template<class T>
bool is_sorted_using_less(const std::vector<T>&v){
	return is_sorted_using_comparator(v, [](const T&l, const T&r){return l < r;});
}

} // RoutingKit

#endif

