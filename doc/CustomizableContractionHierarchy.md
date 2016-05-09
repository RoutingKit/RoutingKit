# CustomizableContractionHierarchy

CustomizableContractionHierarchies (CCH) are an index-based speedup technique for shortest paths in directed graphs that can quickly be adapted to new weights. CCHs use, contrary to regulars CHs, a three phase setup:

1. Preprocessing
2. Customization
3. Query

The preprocessing is slow but does not rely on the arc weights. The Customization introduces the weights and is reasonably fast. Finally, the actual paths are computed in the query phase. A common setup consists of doing the preprocessing once and the customization per user upon login. Further one can use a customization to incorporate live-traffic updates.

All classes and functions are defined in `<routingkit/customizable_contraction_hierarchy.h>`.

## Preprocessing

The central object is the index itself and is called `CustomizableContractionHierarchy`. It is used as following:

```cpp
std::vector<unsigned>tail = ...;
std::vector<unsigned>head = ...;
std::vector<unsigned>node_order = some_expensive_ordering_routine(tail, head);

CustomizableContractionHierarchy cch(node_order, tail, head);
```

`tail` and `head` are represent the input graph and `node_order` is a fill-in reducing node order. All three arguments are copied, i.e., you can destroy them if you want after the constructor is finished. Other than with regular CHs we do not expect the constructor to take too long and therefore do not provide methods to serialize the CCH to the disk. Store the order and rebuild the CCH as needed. There are two further optional parameters. The first is a callback function to get logging messages and the last is a boolean that activates an optimizing that reduces the index size and the query times by exploiting that many one-way streets exist. However, it adds a significant overhead to the CCH construction. By default it is deactivated. The syntax for this further parameters is:

```cpp
CustomizableContractionHierarchy cch1(node_order, tail, head);
CustomizableContractionHierarchy cch2(node_order, tail, head, [](std::string msg){cerr << msg << endl;});
CustomizableContractionHierarchy cch3(node_order, tail, head, [](std::string msg){cerr << msg << endl;}, true);
```

The header `<routingkit/nested_dissection.h>` provides a basic ordering algorithm based upon Inertial Flow. It is fast and provides order of reasonable quality. However, better ordering algorithms such as FlowCutter exist. We plan to incorporate these at some point.

```cpp
std::vector<float>latitude = ...;
std::vector<float>longitude = ...;
std::vector<unsigned>tail = ...;
std::vector<unsigned>head = ...;
unsigned node_count = ...;

std::vector<unsigned>node_order = compute_nested_node_dissection_order_using_inertial_flow(node_count, tail, head, latitude, longitude);
CustomizableContractionHierarchy cch(node_order, tail, head);
```

The ordering function has a final optional parameter that is a logging callback. Note, that using a CH order in a CCH generally does not work well, whereas CCH orders can be used in a CH.

## Customization


The weights are introduced using a helper object called `CustomizableContractionHierarchyMetric`. You use it as following:

```cpp
std::vector<unsigned>weight = ...;
CustomizableContractionHierarchyMetric metric(cch, weight);
metric.customize();
```

The constructor only allocates the memory. It does not actually incorporate the weights. This is what `customize` does. The metric stores a reference to both `cch` and to `weight`, i.e., if either object is destroyed, you are only allowed to destroy `metric` or to call `metric.reset(new_cch, new_weight)` or `metric.reset(new_weight)` in the case that only `weight` was destroyed. `weight` can also be a `const unsigned*` that is interpreted as array. If you modify the weight vector then you must recustomize the metric.

Shortest path queries are computed using a `CustomizableContractionHierarchyQuery` object, as following:

```cpp
CustomizableContractionHierarchyQuery query(metric);
unsigned distance = query.reset().add_source(s).add_target(t).run().get_distance();
```

The interface of `CustomizableContractionHierarchyQuery` is essentially the same as for the corresponding object for regular CHs namely `ContractionHierarchyQuery`. We will therefore not specify the interface here. The object holds a reference to `metric` which means that if `metric` was to be destroyed (or any of the objects that `metric` refers to) then you may only destroy the `query` object or call `query.reset(new_metric)`. Further any of the values in the weight vector referenced in `metric` change then you may not use the query object until the metric has been customized anew. If you customize the metric then the query object will automatically use the new weights.

The method `CustomizableContractionHierarchyMetric::customize` can be too slow for some applications. Two alternative customization methods are therefore provided.

### CustomizableContractionHierarchyParallelization 

The customization can be parallelized as following:

```cpp
CustomizableContractionHierarchyParallelization parallel_customization(cch);
parallel_customization.customize(metric);
// or parallel_customization.customize(metric, thread_count);
```

The parallelization is done using OpenMP primitives, i.e., the internal OpenMP thread pools are used.

Note that the `parallel_customization` constructor computes some auxiliary data. It is therefore probably a good idea to only construct the object once when constructing the `cch` object. If you omit `thread_count` then as many threads are used as processors are available. You can use a single `parallel_customization` object to customize multiple metrics at the same time from different threads. No locking is required. The `parallel_customization` objects holds a reference to `cch`, i.e., if `cch` is destroyed, you need to destroy `parallel_customization` or execute `parallel_customization.reset(new_cch)`.

### CustomizableContractionHierarchyPartialCustomization

Often only a few weights change. The typical application is incorporating a new traffic jam. This can be done as following:

```cpp
std::vector<unsigned>weight = ...;
CustomizableContractionHierarchyMetric metric(cch, weight);
metric.customize();
CustomizableContractionHierarchyPartialCustomization partial_customization(metric);

weight[arc_1] = ...;
weight[arc_2] = ...;
...
weight[arc_n] = ...;

partial_customization
	.reset()
	.update_arc(arc_1)
	.update_arc(arc_2)
	...
	.update_arc(arc_n)
	.customize(metric);
```

The `partial_customization` object is comparatively lightweight but constructing it requires linear running time whereas all other operations run in sub linear time if the CCH does not change too much. It is therefore be a good idea to construct the object only once when constructing the CCH. The `partial_customization` objects holds a reference to `cch`, i.e., if `cch` is destroyed, you need to destroy `partial_customization` or execute `partial_customization.reset(new_cch)`.

### Perfect Customization

In some applications you know that a customization will be followed by a large number of queries. If this number is extremely large then using a regular CH will win as the query running times dominate the time required to build the CH. However, if the number is only large then a compromise exists: Build the CH using a perfect CCH witness search. This works as following:

```cpp
std::vector<unsigned>weight = ...;
CustomizableContractionHierarchyMetric metric(cch, weight);
ContractionHierarchy ch = metric.build_contraction_hierarchy_using_perfect_witness_search();
```

This computes a CH with the same node order as the CCH and is significantly faster than `ContractionHierarchy::build`. However, this approach only works for orders for which you can build a CCH in a reasonable amount of time. These are a subset of the orders for which good CHs can be constructed. Unfortunately, the best CH orders tend to not yield good CCHs. The end result is that for the best CH query running times you need `ContractionHierarchy::build`.

## Publications

* Customizable Contraction Hierarchies.
  Julian Dibbelt, Ben Strasser, and Dorothea Wagner.
  ACM Journal of Experimental Algorithmics, 2016.

For the node orders:

* On Balanced Separators in Road Networks.
  Aaron Schild and Christian Sommer.
  Proceedings of the SEA'15, 2015.

* Graph Bisection with Pareto-Optimization.
  Michael Hamann and Ben Strasser
  Proceedings of the ALENEX'16, 2016.

