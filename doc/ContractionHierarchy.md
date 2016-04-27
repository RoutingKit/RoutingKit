# ContractionHierarchy

ContractionHierarchy (CH) is an index-based speedup technique for shortest path computations. They work in two phases:

1. Preprocessing (or index generation)
2. Query

The preprocessing phase is slow but only depends on the graph and its weights but not on the source and the target nodes. 
The query phase uses the results of the preprocessing phase and is therefore very fast.

All classes and functions are defined in `<routingkit/contraction_hierarchy.h>`.

The central object is the index itself and is called `ContractionHierarchy`. The simplest way to construct it is as following:

```cpp
unsigned node_count = ...;
std::vector<unsigned>tail = ...;
std::vector<unsigned>head = ...;
std::vector<unsigned>weight = ...;

ContractionHierarchy ch = ContractionHierarchy::build(node_count, tail, head, weight);
```

The `build` function copies its parameters. It does not store a reference to them, i.e., you can destroy them after it is finished.

The other important object is called `ContractionHierarchyQuery` and can be used as following:

```cpp
ContractionHierarchyQuery query(ch);

unsigned source_node = ...;
unsigned target_node = ...;

query.reset().add_source(source_node).add_target(target_node).run();
unsigned distance = query.get_distance();
std::vector<unsigned>nodes = query.get_node_path();
std::vector<unsigned>arcs = query.get_arc_path();
```

The variable `distance` stores the shortest path distance from `source_node` to `target_node`.
The other two variables `nodes` and `arcs` represent the path itself. 
`nodes` is a vector of all node IDs in the path whereas `arcs` is a vector of arc IDs.
The ID of an arc is its position in the vectors used to construct the CH.

The reason why the query object is not integrated into the CH itself is to allow an efficient multi-threaded setup. You can have one global CH but a query object per thread.

## Preprocessing

The preprocessing has two optional parameters as illustrated in the following code snipplet:

```cpp
std::function<void(std::string)>log_message = ...;
unsigned max_pop_count = ...;

ContractionHierarchy ch = ContractionHierarchy::build(node_count, tail, head, weight, log_message, max_pop_count);
// or build(node_count, tail, head, weight, log_message);
```

The important parameter is `log_message`. 
As preprocessing can take a long time you might want to have a status message that tells you what stage the algorithm has reached. 
To achieve this `log_message` is called periodically, i.e., to get logging you could use the following code:

```cpp
ContractionHierarchy ch = ContractionHierarchy::build(node_count, tail, head, weight, [](std::string msg){cerr << msg << endl;});
```

The other parameter trades preprocessing time for index quality.
It is best left alone, if you do not know the algorithm's internals.
A small value should provide a fast preprocessing running time but a bad index quality, whereas a large value should give a good index quality but require more preprocessing running time.
However, the world is not that simple. 
We have observed a too small value to be poor with respect to every criterion.
Its default value is `ContractionHierarchy::default_max_pop_count`.

A central component of the preprocessing consists of computing a so-called contraction order. 
This is an ordering of the input nodes. 
A significant fraction of the preprocessing running time is spent computing this order.
It can therefore be beneficial to store this ordering to accelerated the preprocessing.
Further, two graphs that have similar arcs can have very similar contraction orders. 
If you need CHs for both graphs you may therefore compute only a single order and use it for both.
You can access the positions of the nodes in this order using `ch.order`.
The inverse permutation of the order is called rank and can be accessed using `ch.rank`.
Given one of these two permutations, the preprocessing can be accelerated. 
The interface is as following:

```cpp
std::vector<unsigned>order = ...
ContractionHierarchy ch = ContractionHierarchy::build_given_order(order, tail, head, weight);

std::vector<unsigned>rank = ...
ContractionHierarchy ch = ContractionHierarchy::build_given_rank(rank, tail, head, weight);
```

As computing a CH can be slow, we provide functions to save it to disk. 
Before we describe the details of the functions, we want to provide a clear warning:

**Only read CHs from trusted data sources. Manipulated files can be used to execute arbitrary code.**

That being said: We do not expect this to happen by accident. 
The most common unintentional corruptions are detected and others only cause segmentation faults.
We do not fully validate the input for performance reasons.

The simplest functions to use are:

```cpp
ContractionHierarchy ch1 = ...;
ch1.save_file("file_name");
...
ContractionHierarchy ch2 = ContractionHierarchy::load_file("file_name");
```

However, we also provide a more complex interface that allow you to for example the standard iostreams:

```cpp
std::ofstream out("file_name", std::ios_base::binary);
ch.write(out);
```

and

```cpp
std::ifstream in("file_name", std::ios_base::binary);
ContractionHierarchy ch = ContractionHierarchy::read(in);
// or ContractionHierarchy::read(in, file_size);
```

The input function `read` has a second optional parameter which indicates the file size.
If the file size is present then it is checked against the claimed size from the file header and if they do not match then a graceful `std::runtime_error` exception is thrown.
Otherwise the code will go ahead and allocate as much memory as the (possibly corrupt) header states, which can be more than the amount of physical memory and cause your system to become non-reactive do to the OS constantly swapping memory pages from and to the disk.

In some cases you neither want to store the file to disk nor does the data have a standard I/O-stream interface. For this case we provide the following functions:

```cpp
std::ofstream out("file_name", std::ios_base::binary);
ch.write(
	[&](const char*buffer, unsigned long long buffer_size){
		out.write(buffer, buffer_size);
	}
);
...
std::ifstream in("file_name", std::ios_base::binary);
ContractionHierarchy ch = ContractionHierarchy::read(
	[&](char*buffer, unsigned long long buffer_size){ 
		in.read(buffer, buffer_size); 
	}
	// an optional second file_size parameter exists
);
```

It is guaranteed that the callbacks are called only a constant number of times per call to `read` and `write`.
You could for example use these features to stream the CH directly per TCP.

## Query

The basic query interface is 

```cpp
ContractionHierarchyQuery query(ch);
query.reset().add_source(source_node).add_target(target_node).run();
```

The chaining is pure syntactical sugar. You could write this code also as following:

```cpp
ContractionHierarchyQuery query(ch);
query.reset();
query.add_source(source_node);
query.add_target(target_node);
query.run();
```

Constructing a query object is heavy-weight. 
It can therefore be recycled to perform several queries.
Note that the `ch` object is not modified by the `query` object.
A typical multi-threaded server setup therefore consists of one global CH instance and a query object per thread. 

The query object stores a reference to the `ch` object. 
If the `ch` object is destroyed then you must destroy the `query` object or call `query.reset(new_ch)` before using any other `query` method.

To reuse a query object you need to start by calling the `reset` function. 
You can then add the source and target nodes.
Finally, you call `run` to execute the query.
Afterwards, you can use the following functions:

```cpp
unsigned distance = query.get_distance();
std::vector<unsigned>nodes = query.get_node_path();
std::vector<unsigned>arcs = query.get_arc_path();
```

to access the results. Note, that `get_distance` has a constant running time, whereas the two other functions actually need to do some additional computations, which can be as slow as calling `run`, i.e., store the vector in a local variable, if you need to iterate over it several times.

As already stated there can be several source and target nodes. You can write 

```cpp
query.reset().add_source(a).add_source(b).add_target(c).run();
```

which has the meaning that either a path from `a` to `b` or from `a` to `c` (but not both) should be computed depending on which is smaller. 
You can also have multiple targets and even combine multiple sources with multiple targets. 
In the later case the closest pair of source and target nodes is chosen for the path.

To figure out which nodes were the closest you can extract the path and look at it.
This is also the fastest way if you need the path anyway.
However, if you do not plan on querying the path itself, then you can use the following functions:

```cpp
unsigned source = query.get_used_source();
unsigned target = query.get_used_target();
```

Note, that while being cheaper than the functions that extract the paths, `get_used_source` and `get_used_target` perform a partial path extraction and therefore do not have a constant running time.

It is possible that you do not want to compute the closest pair but want to penalize some nodes. 
You can do this as following:

```cpp
unsigned dist_to_a = ...;
unsigned dist_to_b = ...;
query.reset().add_source(a, dist_to_a).add_source(b, dist_to_b).add_target(c).run();
```

The meaning of this is that every path from `a` is regarded as being `dist_to_a` longer than it actually is.
If no penalty is provided then it is implicitly zero.
You can do the same for targets.

This penalty is added to the value returned by `get_distance`. Note that if you sum up the weights of arcs returned by `get_arc_path` and use a non-zero penalty then the sum will not match the value returned by `get_distance`. The reason is that the sum does not account for the penalties while `get_distance` does.

# Many-to-Many Queries

You can also use the normal `ContractionHierarchyQuery` object to compute one-to-many and many-to-one queries. 
Iteratively calling these gives you many-to-many queries.

Note that a design goal was to not use more memory per query object than was needed to answer one-to-one queries. This slightly limits what algorithms we can employ. It is therefore probably possible to design slightly faster specialized many-to-many query objects that use more memory. 

The basic usage pattern is as following:

```cpp
ContractionHierarchyQuery query(ch);
std::vector<unsigned>source_list = ...;
std::vector<unsigned>target_list = ...;

query.reset().pin_targets(target_list);
	
for(auto s:source_list){
	std::vector<unsigned> d = query.reset_source().add_source(s).run_to_pinned_targets().get_distances_to_targets();
	// d[i] contains the distance from s to target_list[i]
}
```

Instead of pinning targets, you can also pin the source nodes as following:

```cpp
ContractionHierarchyQuery query(ch);
std::vector<unsigned>source_list = ...;
std::vector<unsigned>target_list = ...;

query.reset().pin_sources(source_list);
	
for(auto t:target_list){
	std::vector<unsigned> d = query.reset_target().add_target(t).run_to_pinned_sources().get_distances_to_sources();
	// d[i] contains the distance from source_list[i] to t
}
```

You can also combine this with multiple calls to `add_source` or `add_target` as the following code snippet illustrates:

```cpp
unsigned dist_to_a = ...;
unsigned dist_to_b = ...;
std::vector<unsigned> d = query
	.reset()
	.pin_targets(target_list)
	.add_source(a, dist_to_a)
	.add_source(b, dist_to_b)
	.add_source(c)
	.run_to_pinned_targets()
	.get_distances_to_targets();
```

`d[i]` then contains the minimum of `dist(a, target_list[i])+dist_to_a` and `dist(b, target_list[i])+dist_to_b` and `dist(c, target_list[i])` where `dist(x,y)` is the the shortest path distance from node `x` to node `y`.

