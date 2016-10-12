# Caching the index creation

RoutingKit is able to generate an index from a graph of moderate size in less than 60s while starting an application. For development, shared systems and online webservices it may be more efficient to precompute this index and share the result to save computing time.

Saving the index to disk is as easy as:
```cpp
auto graph = simple_load_osm_car_routing_graph_from_pbf("osm.pbf");
auto tail = invert_inverse_vector(graph.first_out);
auto ch = ContractionHierarchy::build(
    graph.node_count(),
    tail, graph.head,
    graph.travel_time
);
ch.save_file("/tmp/routingkit.index");
```

Loading the index is as easy as:
```cpp
auto ch = ContractionHierarchy::load_file("/tmp/routingkit.index");
```
