# How Graphs are represented

Graphs in RoutingKit are always directed and are represented either as arc-list or as adjacency-array. An arc-list has the following structure:

```cpp
unsigned node_count;
std::vector<unsigned>tail;
std::vector<unsigned>head;
```

`node_count` stores, as the name suggest, the number of nodes in the graph. `tail` and `head` are two vectors of equal size. They should contain for every arc its tail, i.e., the node from which the arc departs, respectively, its head, i.e., the node where the arc arrives. IDs in RoutingKit are always unsigned 32-bit integers. They range from 0 to number of objects minus one. In this case this means that all values in `tail` and `head` are in the range `0` to `node_count-1`. The position of the arc in the array is its arc ID. Besides `tail` and `head` an arc can have further properties. Common ones are:

```cpp
std::vector<unsigned>geo_distance;
std::vector<unsigned>travel_time;
std::vector<unsigned>speed;
```

These represent the geographic distance, the travel_time, and the speed respectively. Unless stated otherwise, geographical distances are always in meter, travel times in seconds, and speeds in km/h.
 
RoutingKit supports any graph representable under this form. This includes border cases such as graphs with multi-arc, loops, disconnected graphs, or zero-weights. If a RoutingKit function does not properly behave given these inputs then we consider it a bug.

Arc-lists are simple and easy to use and excel at most tasks except graph traversal. For this cases we use an adjacency-array. It has the following form:

```cpp
std::vector<unsigned>first_out;
std::vector<unsigned>head;
```

Before specifying the details we'd like to show the most frequent usage pattern. It illustrates the structure very well. The most frequent usage of an adjacency-array is iterating over the outgoing arcs of a node, as follows:

```cpp
for(unsigned xy=first_out[x]; xy<first_out[x+1]; ++xy){
	unsigned y = head[xy];
	// xy is the arc from node x to node y
}
```

`first_out` is a vector with `node_count+1` elements. The first element must be zero. The last element is the number of arcs. Converting an adjacency array to an arc list can be done using functions from the header `<routingkit/inverse_vector.h>` as following:

```cpp
unsigned node_count = first_out.size()-1;
auto tail = invert_inverse_vector(first_out);
```

The conversion from an arc-list to an adjacency-array is less simple. It requires the arcs to be sorted by tails, which they often are not. Sorting can be done as follows:

```cpp
auto input_arc_id = compute_sort_permutation_using_less(tail);
tail = apply_permutation(input_arc_id, tail);
head = apply_permutation(input_arc_id, head);
travel_time = apply_permutation(input_arc_id, travel_time);
```

Once the arcs appear in the correct order you can compute the adjacency array as following:

```cpp
auto first_out = invert_vector(tail, node_count);
```

The core of an adjacency-array consists of a special way to store a sorted vector of IDs. This concept is applicable in a significantly broader set of cases than only representing graphs. We refer to this structure as inverse vector.

# Permutations

Permutations are used over and over in RoutingKit and therefore a number of auxiliary functions are provided in `<routingkit/permutation.h>`. A permutation of `n` elements is represented as `vector<unsigned>` where all numbers from `0` to `n-1` appear exactly once. You can permutate a set of elements using the `apply_permutation` function as following:

```cpp
vector<string>input_names = {"banana", "pear", "pineapple", "strawberry", "apple"};
vector<unsigned>p = compute_sort_permutation_using_less(names);
output_names = apply_permutation(p, names);

for(unsigned i=0; i<p.size(); ++i)
	assert(output_names[i] == input_name[p[i]]);
```

You can check whether a vector is a valid permutation using `is_permutation` as follows:

```cpp
vector<unsigned>p = {3,0,2,1};
assert(is_permutation(p));
p = {0,0,1,2};
assert(!is_permutation(p));
p = {1,2};
assert(!is_permutation(p));
```

You can invert permutations using `invert_permutation` as follows:

```cpp
vector<unsigned>p = {3,0,2,1};
vector<unsigned>inv_p = invert_permutation(p);

assert(inv_p[0] == 1);
assert(inv_p[1] == 3);
assert(inv_p[2] == 2);
assert(inv_p[3] == 0);
```

Calling `invert_permutation` can often be avoided. For most functions there are variants that operate directly using the inverse permutation. For example the example code above can be rewritten as:

```cpp
vector<string>input_names = {"banana", "pear", "pineapple", "strawberry", "apple"};
vector<unsigned>inv_p = compute_inverse_sort_permutation_using_less(names);
output_names = apply_inverse_permutation(inv_p, names);

for(unsigned i=0; i<p.size(); ++i)
	assert(output_names[inv_p[i]] == input_name[i]);
```

You can get the identity permutation on n elements using `identity_permutation(n)` as follows:

```cpp
vector<unsigned>p = identity_permutation(42);

for(unsigned i=0; i<p.size(); ++i)
	assert(p[i] == i);
```

You can generate a random permutation using `random_permutation(n, rand_gen)` where `rand_gen` is a random number generator as those used by the C++ `<random>` header. For example:

```cpp
minstd_rand gen;
vector<unsigned>p = random_permutation(42, gen);

assert(is_permutation(p));
```

You can chain permutations using `chain_permutation_first_left_then_right` and `chain_permutation_first_right_then_left` as follows:

```cpp
minstd_rand gen;
vector<string>input_names = {"banana", "pear", "pineapple", "strawberry", "apple"};
vector<unsigned>p = random_permutation(input_names.size(), gen);
vector<unsigned>q = random_permutation(input_names.size(), gen);

vector<unsigned>qp = chain_permutation_first_left_then_right(p, q);
vector<unsigned>pq = chain_permutation_first_right_then_left(p, q);

for(unsigned i=0; i<p.size(); ++i)
	assert(input_names[qp[i]] == input_names[q[p[i]]]);

for(unsigned i=0; i<p.size(); ++i)
	assert(input_names[pq[i]] == input_names[p[q[i]]]);
```

Sometimes you do not want to reorder the elements in a vector but the elements are IDs and you want to reorder these. Here you can use `apply_permutation_to_elements_of` and `inplace_apply_permutation_to_elements_of`. The most common usecase in RoutingKit consists of reordering the nodes in a graph:

```cpp
vector<string>node_name = {"Entenhausen", "Gotham City", "Springfield", "Bielefeld", "Hogwarts"};
vector<unsigned>inv_p = compute_inverse_sort_permutation_using_less(node_name);

node_name = apply_inverse_permutation(inv_p, node_name);
tail = apply_permutation_to_elements_of(inv_p, tail);
inplace_apply_permutation_to_elements_of(inv_p, head);
```

If the IDs in the vector can be invalid and set to `invalid_id` then you can use `inplace_apply_permutation_to_possibly_invalid_elements_of` and `apply_permutation_to_possibly_invalid_elements_of` which leave these elements unchanged.

# Sorting

`std::sort` is nice to sort vectors of arbitrary structs but that is not a common usecase in RoutingKit, where we prefer having a vector per attribute. Further, we nearly always sort vectors of IDs. This can usually be done faster using bucket sort than with a general purpose sorter. For this reason the header `<routingkit/sort.h>` provides a number of helper functions. Instead of explaining every function it is better to explain their naming scheme which is very systematic. All functions fall into one of three categories:

* `compute_[inverse_][stable_]sort_using_[(key|less|comparator](vector, ...order...)`
* `[stable_]sort_using_[key|less|comparator](vector, ...order...)`
* `is_sorted_using_[key|less|comparator](vector, ...order...)`

The function from the first category compute a sort permutation but do not reorder the elements in any vector. The functions from the second category create a copy of the vector given as argument, sort the copy, and return the sorted copy. Finally, the last category of function does not sort anything but checks whether something is sorted.

A sort is stable if it preserves the relative order of equal elements. A sort permutation `p` of a vector `v` is one such that `v[p[0]] <= v[p[1]] <= v[p[2]] <= ... `. You can directly compute the inverse permutation, which is faster than the equivalent combination of `invert_permutation` and `compute_sort_using_*`.

You can sort by three criteria. The additional order parameters and the underlying algorithm depend on the criterion used. `using_less` means that the global operator< should be used. No arguments beside the vector are needed. `using_comparator` means that a comparator as the one used by `std::sort` is provided as only additional parameter. Finally `using_key` first maps all elements only ID keys in the range `0` to `key_count-1` using a function object `get_key` and sorts the elements using the key increasingly. The first additional parameter is `key_count` and the second one is `get_key`.

The `using_key` functions use bucket sort and therefore guarantee that their running time is linear in `key_count`. This is for example useful to quickly sort the arcs to build an adjacency array:

```cpp
auto p = compute_inverse_sort_permutation_using_key(tail, node_count, [](unsigned x){return x;});
tail = apply_inverse_permutation(p, tail);
head = apply_inverse_permutation(p, head);
weight = apply_inverse_permutation(p, weight);
auto first_out = invert_vector(tail, node_count);
```

Being able to perform a stable sort also has an important usecase. Sorting by tail is enough to build an adjacency array. However, often we also want to sort the heads as secondary criterion. This can be done in linear running time as illustrated in the following code snippet:

```cpp
// first sort by head
{
	auto p = compute_inverse_sort_permutation_using_key(head, node_count, [](unsigned x){return x;});
	tail = apply_inverse_permutation(p, tail);
	head = apply_inverse_permutation(p, head);
	weight = apply_inverse_permutation(p, weight);
}
// next use a stable sort by tail
{
	auto p = compute_inverse_stable_sort_permutation_using_key(tail, node_count, [](unsigned x){return x;});
	tail = apply_inverse_permutation(p, tail);
	head = apply_inverse_permutation(p, head);
	weight = apply_inverse_permutation(p, weight);
}
auto first_out = invert_vector(tail, node_count);
for(unsigned x=0; x<node_count; ++x)
	for(unsigned xy=first_out[x]+1; xy < first_out[x+1]; ++xy)
		assert(head[xy-1] <= head[xy]);
```

This usecase is so common that there is a special function for it called `compute_[inverse_]sort_permutation_first_by_tail_then_by_head` for it is provided in `<routingkit/graph_util.h>`. It can be used as following:

```cpp
{
	auto p = compute_inverse_sort_permutation_first_by_tail_then_by_head(node_count, tail, head);
	tail = apply_inverse_permutation(p, tail);
	head = apply_inverse_permutation(p, head);
	weight = apply_inverse_permutation(p, weight);
}
auto first_out = invert_vector(tail, node_count);
for(unsigned x=0; x<node_count; ++x)
	for(unsigned xy=first_out[x]+1; xy < first_out[x+1]; ++xy)
		assert(head[xy-1] <= head[xy]);
```

This usecase also nearly always involes permutating `tail`, which can be exploited to accelerate the overall processing a bit. We therefore also provide `compute_[inverse_]sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail`. It can be used as following:

```cpp
{
	auto p = compute_inverse_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(node_count, tail, head);
	head = apply_inverse_permutation(p, head);
	weight = apply_inverse_permutation(p, weight);
}
auto first_out = invert_vector(tail, node_count);
for(unsigned x=0; x<node_count; ++x)
	for(unsigned xy=first_out[x]+1; xy < first_out[x+1]; ++xy)
		assert(head[xy-1] <= head[xy]);
```

For the significantly less common case that the tail and the head IDs are from different ID ranges we also provide `compute_[inverse_]sort_permutation_first_by_left_then_by_right[_and_apply_sort_to_left](left_count, left_vector, right_count, right_vector)`.

# BitVector, IDMapper and Filter

`BitVector` is a class defined in `<routingkit/bit_vector.h>` that, as the name suggests, stores for every element one bit of information. It is similar to `std::vector<bool>` but has the advantage that it allows access to the underlying array. This is crucial in being able to quickly serialize a bit vector to disk. This direct access further allows us to make use of certain processor instructions such as the instruction `popcnt` which counts how many bits are set in an unsigned integer. You can directly manipulate the internal array of a BitVector. It is guaranteed to be a multiple of 512 bits and to be aligned on a 512 bit boundary. Further, all padding bits are guaranteed to be zero.

You can use a `BitVector` to filter the elements of another vector. The header `<routingkit/filter.h>` provides the following four functions which do exactly what their name suggests:

```cpp
std::vector<T>keep_element_of_vector_if(const BitVector&, std::vector<T>);
std::vector<T>remove_element_of_vector_if(const BitVector&, std::vector<T>);
void inplace_keep_element_of_vector_if(const BitVector&, std::vector<T>&);
void inplace_remove_element_of_vector_if(const BitVector&, std::vector<T>&);
```

Usually it is better to think positive and mark the elements that you want to keep instead of those that you want to remove. If you do so, then you can make use of `LocalIDMapper` and `IDMapper` from the header `<routingkit/id_mapper.h>. These can be used as following:

```cpp
vector<string> names = {"Bob", "Alice", "Charlie", "Malice"};
BitVector keep_filter = make_bit_vector(names.size(), [&](unsigned i){return names[i][1] != 'a';});

LocalIDMapper map(keep_filter);

vector<string> filtered_names = keep_element_of_vector_if(keep_filter, names);

for(unsigned i=0; i<names.size(); ++i){
	if(keep_filter.is_set(i))
		assert(filtered_names[map.to_local(i)] == names[i]); 
}
```

The usecase above could also be solved using a `std::vector<unsigned>` that maps the old IDs onto the new IDs or onto `invalid_id` if a name was removed. Indeed, this transformation is always possible but can require significantly more memory than `LocalIDMapper` which requires less than one bit per element in addition to the bit used by `keep_filter`. This can be a game changer in situation, where you have a very large ID range, such as OSM node ids, and you need to map these onto a smaller ID range, such as the OSM nodes actually used for routing. Contrary to most of RoutingKit, `LocalIDMapper` works with 64 bit integers. `to_local(x)` must only be called for values where `keep_filter` is true. The is also `to_local(x, default_value)` which returns `default_value` if `x` is removed. There is also `is_global_id_mapped(x)`, which is equivalent to `keep_filter.is_set(x)`, `local_id_count()`, and `global_id_count()` which do exactly what their names suggest. 

The running time of `to_local` is nearly indistinguishable from an array lookup, if your processor supports the `popcnt` instruction. If this instruction is not available, then `to_local` is still fast, but expect a major performance penalty.

`IDMapper` provides all functionality that `LocalIDMapper` provides but requires a little bit more memory to additionally provide a `to_global` function that allows you to map an ID from the small range onto the corresponding ID from the large range. Note that while `to_global` is still fast, it is noticably slower than `to_local`.

`LocalIDMapper` and `IDMapper` store a pointer to `keep_filter` and therefore you must make sure to not prematurely destroy `keep_filter`. Once `keep_filter` is destroyed you may no longer call `to_local`, `to_global`, and `is_global_id_mapped`.


