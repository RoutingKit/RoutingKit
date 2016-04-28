# Converting Coordinates to Node IDs

Geographical positions can be converted routing node IDs by searching for the node that is the closest. These queries are called nearest neighbor searches. A basic implementation consists of computing the distances to every node but this is too slow in practice. An index-based approach is necessary. RoutingKit therefore provides a Vantage-Point Tree implementation. It can be used as following:

```cpp
#include <routingkit/geo_position_to_node.h>

...
std::vector<float>latitude_of_node = ...;
std::vector<float>longitude_of_node = ...;
GeoPositionToNode index(latitude_of_node, longitude_of_node);
...
float query_latitude = ...;
float query_longitude = ...;
float query_radius = 1000;

auto r = index.find_nearest_neighbor_within_radius(query_latitude, query_longitude, query_radius);
if(r.id == invalid_id)
	cout << "No node was found within "<<query_radius<<" meter." << endl;
else
	cout << "Node "<<r.id<<" was found at distance "<<r.distance<<" meter." << endl;
```

The query consists of finding the closest node that is not further away then a given upper bound. In the most common setting it is useful to use some constant for the search radius such as for example 1km.

The implementation of RoutingKit measures distances along the Earth surface (or to be more precise, an approximating sphere). A consequence of this is that the query will also be correct in border cases such as the Earth poles or the longitude wrap-around in the pacific. Contrary to many other implementation RoutingKit uses a vantage-point tree and not a kd-tree.

## Publications

* Data structures and algorithms for nearest neighbor search in general metric spaces.
  Peter N. Yianilos.
  Proceedings of the fourth annual ACM-SIAM Symposium on Discrete algorithms, 1993.
* Satisfying General Proximity/Similarity Queries with Metric Trees.
  Jeffrey Uhlmann.
  Information Processing Letters, 1991.

