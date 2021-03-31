RoutingKit includes an OpenStreetMap (OSM) data importer. The importer reads PBF files and is optimized for speed and memory consumption. OSM contains the map data. How this data should interpreted is left to the user and depends on the application. For example, the extracted routing graph is significantly different, if one wants to route pedestrians or cars. Pedestrians can use stairs while cars cannot. Cars can drive 130 km/h on the freeway, while pedestrians and not even allowed on the freeway. 

The meta information attached to OSM objects is encoded in a key-value-form. For example, a stair is encoded using "highway=steps". RoutingKit separates the OSM data decoding from the interpretation of the data. The decoding step figures out that there is a tag "highway=steps". The interpretation step determines that "highway=steps" means that no cars can pass.

In many situations, one does not care about the exact interpretation and wants to have something that "just works". RoutingKit therefore provides very simple but inflexible functions in the header `<routingkit/osm_simple.h>`. If you just want a graph, this is the header for you.

OSM contains many nodes. A lot of these encode for example the contours of buildings. These are not relevant for routing. RoutingKit therefore does not export every node. It discerns between three types of nodes:

* OSM nodes
* Modelling nodes
* Routing nodes

The set of routing nodes is a subset of the modelling nodes which are a subset of the OSM nodes. Routing nodes are the nodes in the extracted routing graph. Modelling nodes are used to represent the geometry of a road. In OSM, a way does not have a geometry attribute as is the case for many other map datasets. Instead, the curvature of a road is encoded using many nodes of degree two. These nodes are not relevant for routing as no routing decision takes place at them. There are therefore not part of the routing graph. They are referred to as modelling nodes. 

Modelling nodes are considered when computing the distance of an arc. Currently, they are just discard afterwards. The long term goal is to compress their geographic positions into an arc attribute. There is a boolean parameter that turns all modelling nodes into routing nodes to work around this current limitation.

Routing nodes have consecutive integer IDs. The first node has ID 0. Their relative order corresponds to the OSM node order. RoutingKit stores OSM IDs using 64 bits but assumes that the number of modelling and routing nodes fits into a 32 bit integer.

There are further two type of way IDs:

* OSM ways
* Routing ways

An OSM way ID is the ID exactly as it appears in OSM. Only ways that are used for routing are routing ways. They are assigned consecutive IDs that are called routing way ID.

Currently, RoutingKit provides interpretations for a standard car, a standard pedestrian, and a standard bicycle.

# Simplified Interface

`<routingkit/osm_simple.h>` contains three functions. They are called:

```cpp
SimpleOSMCarRoutingGraph simple_load_osm_car_routing_graph_from_pbf(const std::string&pbf_file, const std::function<void(const std::string&)>&log_message = nullptr, bool all_modelling_nodes_are_routing_nodes = false, bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false);
SimpleOSMPedestrianRoutingGraph simple_load_osm_pedestrian_routing_graph_from_pbf(const std::string&pbf_file, const std::function<void(const std::string&)>&log_message = nullptr, bool all_modelling_nodes_are_routing_nodes = false, bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false);
SimpleOSMBicycleRoutingGraph simple_load_osm_bicycle_routing_graph_from_pbf(const std::string&pbf_file, const std::function<void(const std::string&)>&log_message = nullptr, , bool all_modelling_nodes_are_routing_nodes = false, bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false);
```

All functions take up to three parameters. The parameters are:

* `pbf_file` is string that contains the path to the PBF file.
* `log_message` is a callback that is invoked with logging messages. If it is null, no logging messages are generated. This is marginally faster than discarding logging messages using a do-nothing callback.
* `all_modelling_nodes_are_routing_nodes` is a boolean that determines whether all modelling nodes should be a routing node.
* The final fourth parameter `file_is_ordered_even_though_file_header_says_that_it_is_unordered` is a boolean to work around a bug in PBF data from some sources. The PBF file can be sorted or not. Sorted PBF can be read significantly faster. In theory, PBFs have a header in which they indicate, whether they are sorted. Unfortunately, some data sources provide sorted files without setting the flag. By passing `true` this header check is ignored and the file is always assumed to be sorted. By default this parameter is `false`.

The two last parameters might disappear in future RoutingKit releases.
 
The car routing function returns a struct with the following members:

```cpp
struct SimpleOSMCarRoutingGraph{
  std::vector<unsigned>first_out;
  std::vector<unsigned>head;
  std::vector<unsigned>travel_time;
  std::vector<unsigned>geo_distance;
  std::vector<float>latitude;
  std::vector<float>longitude;
  std::vector<unsigned>forbidden_turn_from_arc;
  std::vector<unsigned>forbidden_turn_to_arc;

  unsigned node_count()const;
  unsigned arc_count()const;
};
```

The vectors `first_out` and `head` form a graph in the format described in [SupportFunctions]. There are two edge weights provided, namely `travel_time` which is in milliseconds and `geo_distance` which is in meter. The geographical positions of every node are given by `latitude` and `longitude`. Both are in degree, i.e., every latitude is between -90 and +90 and every longitude between -180 and +180. 

Turn restrictions are given by `forbidden_turn_from_arc` and `forbidden_turn_to_arc`. A turn restriction is for example a no-right-turn-sign. An arc path is invalid, if it contains the two arcs `forbidden_turn_from_arc[i]` and `forbidden_turn_to_arc[i]` consecutively in this order for any `i`. The vectors are first sorted by from arc ID and then by to arc ID. It is guaranteed that there are no duplicates.

The functions `node_count` and `arc_count` are just handy function to access the corresponding values.

The pedestrian routing function returns a struct with the following members:

```cpp
struct SimpleOSMPedestrianRoutingGraph{
  std::vector<unsigned>first_out;
  std::vector<unsigned>head;
  std::vector<unsigned>geo_distance;
  std::vector<float>latitude;
  std::vector<float>longitude;

  unsigned node_count()const;
  unsigned arc_count()const;
};
```

Their meaning is exactly the same as for the car routing graph. 

The bicycle routing function returns the following struct:

```cpp
struct SimpleOSMBicycleRoutingGraph{
  std::vector<unsigned>first_out;
  std::vector<unsigned>head;
  std::vector<unsigned>geo_distance;
  std::vector<float>latitude;
  std::vector<float>longitude;
  std::vector<unsigned char>arc_comfort_level;

  unsigned node_count() const;
  unsigned arc_count() const;
};
```

It is very similar to the pedestrian graph, except that there additionally exists an arc value `arc_comfort_level`.

A high comfort means that the way is nice, i.e., for example a road specifically designed for bicycles. A low comfort level means, that while one is technically allowed to cycle, one wants to avoid these roads. For example, a large road with a lot of cars and no bicycle lane has a low comfort. The minimum comfort can be accessed by calling the global `get_min_bicycle_comfort_level()` function. Analogously, the maximum comfort is given by `get_max_bicycle_comfort_level()`. 

The comfort levels do not have a useful scale. A level of 4 only means that it is better than a 2. One should not infer that it is "twice" as good.

# Decoding Interface

The low-level PBF parsing code is in `<routingkit/osm_decode.h>`. It consist of two functions:

```cpp
void unordered_read_osm_pbf(
  const std::string&file_name,
  std::function<void(uint64_t osm_node_id, double latitude, double longitude, const TagMap&tags)>node_callback,
  std::function<void(uint64_t osm_way_id, const std::vector<uint64_t>&osm_node_id_list, const TagMap&tags)>way_callback,
  std::function<void(uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags)>relation_callback,
  std::function<void(const std::string&msg)>log_message = [](const std::string&){}
);

void ordered_read_osm_pbf(
  const std::string&file_name,
  std::function<void(uint64_t osm_node_id, double latitude, double longitude, const TagMap&tags)>node_callback,
  std::function<void(uint64_t osm_way_id, const std::vector<uint64_t>&osm_node_id_list, const TagMap&tags)>way_callback,
  std::function<void(uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags)>relation_callback,
  std::function<void(const std::string&msg)>log_message = [](const std::string&){},
  bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false
);
```

The first function scans a PBF file and calls a callback for every element found in the PBF file. You can pass null as callback, if you do not require the elements of a certain category. Passing null is better than a do-nothing function as the decoder is intelligent enough to not decode elements of a certain type if the corresponding callback is null. `log_message` and `file_is_ordered_even_though_file_header_says_that_it_is_unordered` have the same meaning as for the simplified interfaces. 

The unordered function variant invokes the callbacks in the the order as they appear in the file. It performs one scan over the data.

The ordered function first enumerates all nodes, then all ways, and finally all relations. If the PBF file is sorted, it can do this in one scan. If it is not sorted, then the function does three scans of the input file.

Both functions stream the data from disk. This means that PBFs significantly larger that the available RAM can be read.

All callbacks are handed a `TagMap` object. This is an efficient hash map implementation. It is implemented in `<routingkit/tag_map.h>`. Usually, one does not need to construct objects of this type. In the following, we therefore only describe the access functions. For the remaining functions, we refer to the header.

```cpp
class TagMap{
public:
  const char*operator[](const char*key) const;
  bool empty() const;
  unsigned size() const;

  struct Entry{
    const char*key;
    const char*value;
  };
  
  const_random_access_entry_iterator begin() const;
  const_random_access_entry_iterator end() const;
};
```

The tag maps returns pointers into its internal storage. You must not modify the corresponding strings. (They are `const` after all!) All pointer are invalidated once the TagMap is destroyed.

The `empty` function returns true if there are no key-value-pairs. `size` returns the number of pairs. The `begin` and `end` functions provide iterates over all key-value-pairs. The order in which the pairs are enumerated is unspecified and may change.

The most useful function is the operator[]. It is passed a key and if a corresponding pair exists it returns the value. If no key exists it returns null. This lookup is fast but not for free. A very common pattern is therefore the following:

```cpp
const char*highway = tag_map["highway"];
if(highway){
  if(!strcmp(highway, "steps")){
    // way is good for pedestrians but bad for cars
  }
}
```

The node callback is passed the geographic position of the node. The way callback is passed a vector containing the OSM node IDs of the nodes in the way. The relation callback is passed a list of members of type `OSMRelationMember` which is defined as following:

```cpp
enum class OSMIDType{
  node,
  way,
  relation
};

struct OSMRelationMember{
  OSMIDType type;
  uint64_t id;
  const char*role;
};
```

Every member can either be a node, a way, or another relation. This is indicated by the `type` attribute. The `id` attribute is the OSM ID of the corresponding object. `role` is a string that describes the OSM role of the object in this relation. 

# Graph Decoding Interface

RoutingKit contains special functions to extract routing graphs. Include `<routingkit/graph_builder.h>` to access it. It works in two phases. In the first phase, RoutingKit decides which nodes are modelling and which routing nodes. It further decides which ways are part of the routing graph. At the end of this phase RoutingKit assigns consecutive IDs to all relevant objects. The first step consists of the following function:

```cpp
struct OSMRoutingIDMapping{
  BitVector is_modelling_node;
  BitVector is_routing_node;
  BitVector is_routing_way;
};

OSMRoutingIDMapping load_osm_id_mapping_from_pbf(
  const std::string&pbf_file_name,
  std::function<bool(uint64_t osm_node_id, const TagMap&node_tags)>is_routing_node,
  std::function<bool(uint64_t osm_way_id, const TagMap&way_tags)>is_way_used_for_routing,
  std::function<void(std::string)>log_message = nullptr,
  bool all_modelling_nodes_are_routing_nodes = false
);
```

The function reads a PBF and classifies the objects. The classification is returned. There is a bit for every OSM object. If this bit is true, the corresponding object has this property. A routing way is a way that is part of the routing graph. A modelling node lies on one way. A routing node lies on two ways. The `is_way_used_for_routing` callback determines whether a OSM way should be part of the routing node. The `is_routing_node` callback can be used to make nodes routing nodes even though they do not lie on multiple ways. This can be useful to make sure that for example bus stops are part of the routing graph. The `is_routing_node` callback can be null. This is interpreted as a callback that always returns false.

You can use `OSMRoutingIDMapping` to map IDs from and to the corresponding OSM IDs. Use `IDMapper` to achieve this as follows:

```cpp
OSMRoutingIDMapping mapping = ...;
IDMapper routing_node_mapper(mapping.is_routing_node);
unsigned routing_id;
uint64_t osm_id = ...;
routing_id = routing_node_mapper.to_local(osm_id);
osm_id = routing_node_mapper.to_global(routing_id);
```

The core of the second step is the following function:

```cpp
struct OSMRoutingGraph{
  std::vector<unsigned>first_out;
  std::vector<unsigned>head;
  std::vector<unsigned>way;
  std::vector<unsigned>geo_distance;
  std::vector<float>latitude;
  std::vector<float>longitude;
  std::vector<bool>is_arc_antiparallel_to_way;
  std::vector<unsigned>forbidden_turn_from_arc;
  std::vector<unsigned>forbidden_turn_to_arc;
  std::vector<unsigned>first_modelling_node;
  std::vector<float>modelling_node_latitude;
  std::vector<float>modelling_node_longitude;

  unsigned node_count()const;
  unsigned arc_count()const;
};

OSMRoutingGraph load_osm_routing_graph_from_pbf(
  const std::string&pbf_file,
  const OSMRoutingIDMapping&mapping,
  std::function<
    OSMWayDirectionCategory(
      uint64_t osm_way_id, 
      unsigned routing_way_id, 
      const TagMap&way_tags
    )
  >oneway_classifier,
  std::function<
    void(
      uint64_t osm_relation_id, 
      const std::vector<OSMRelationMember>&member_list, 
      const TagMap&tags, 
      std::function<void(OSMTurnRestriction)>on_new_turn_restriction
    )
  >turn_restriction_classifier,
  std::function<void(const std::string&)>log_message = nullptr,
  bool file_is_ordered_even_though_file_header_says_that_it_is_unordered = false,
  OSMRoadGeometry geometry_to_be_extracted = OSMRoadGeometry::none
);
```

It takes the PBF file as argument and the ID mappings returned by `load_osm_id_mapping_from_pbf`. Besides the two parameters described in the basic decoding interface documentation, there are two additional callbacks and a parameter affecting the extraction of road geometry data. It returns a routing graph with a few additional attributes.

`first_out` and `head` form a directed graph. The node IDs used in this graph are routing IDs. `way` maps an arc ID onto the routing way ID, and `is_arc_antiparallel_to_way` indicates whether the arc runs parallel or antiparallel to the way. `geo_distance` maps an arc onto the length of the arc in meter. `latiude` and `longitude` map routing node IDs onto their geographical positions. Latitude is a number between -90 and +90. Longitude is a number between -180 and 180.

`forbidden_turn_from_arc` and `forbidden_turn_to_arc` encode forbidden turns. If they are empty, there are no forbidden turns. For every `i`, the pair `forbidden_turn_from_arc[i]` and `forbidden_turn_to_arc[i]` is a forbidden turn. This means that every arc path that contains both arcs consecutively is invalid. The vectors are guaranteed to be sorted first by from arc ID and second by to arc ID. It is further guaranteed that there are no duplicate entries. As they are sorted, one can employ the following pattern to determine whether a specific turn is forbidden:

```cpp
auto first_forbidden_turn = invert_vector(forbidden_turn_from_arc, arc_count);
auto is_forbidden = [&](unsigned from_arc_id, unsigned to_arc_id){
  for(unsigned i=first_forbidden_turn[from_arc_id]; i!=first_forbidden_turn[from_arc_id+1]; ++i)
  if(forbidden_turn_to_arc[i] == to_arc_id)
    return true;
  return false;
};
```

The last three data members store road geometry data. Each arc starts at a routing node and ends at a routing node. Between its tail and head node, there can be arbitrarily many modelling nodes. The vectors `modelling_node_latitude` and `modelling_node_longitude` store the latitudes and longitudes of all modelling nodes that are no routing nodes. For each arc `a`, `first_modelling_node[a]` is the index in `modelling_node_latitude` and `modelling_node_longitude` of the first modelling node of `a`, directly following its tail node. Analogously, `first_modelling_node[a + 1] - 1` is the index of the last modelling node of `a`, directly preceding its head node. Note that all three data members are empty by default. To fill them, set the parameter `geometry_to_be_extracted` to `OSMRoadGeometry::uncompressed`.

The first callback is `oneway_classifier` which determines whether a way is a one-way street. If the callback is null, all ways are open in both directions. The callback must return a value of the following enum:

```cpp
enum class OSMWayDirectionCategory{
  open_in_both,
  only_open_forwards,
  only_open_backwards,
  closed
};
```

A way that is open in forward direction is open in the direction in which OSM gives the nodes. Backward means that way is open in the opposite direction. Closed means that both directions is closed. This is different than returning false in the `is_way_used_for_routing` callback. If `is_way_used_for_routing` returns false, the corresponding nodes are not turned into modelling or routing nodes whereas they are, if `oneway_classifier` returns `OSMWayDirectionCategory::closed`.

If for some reason you need to extract additional information about ways, such as for example what way is a tunnel, extract this information in the oneway_classifier. Allocate an array that maps routing way IDs onto the required information.

The `turn_restriction_classifier` determines whether a relation is a turn restriction. If the callback is null, no turn restrictions are extracted. A single relation can be expanded into multiple restrictions. The callback is thus passed a callback `on_new_turn_restriction` which should be called for every restrictions. If `on_new_turn_restriction` is not called, no restriction is generated. The parameter of `on_new_turn_restriction` is a value of the following struct:

```cpp
enum class OSMTurnRestrictionCategory{
  mandatory,
  prohibitive
};

struct OSMTurnRestriction{
  uint64_t osm_relation_id;
  OSMTurnRestrictionCategory category;
  uint64_t from_way;
  uint64_t via_node;
  uint64_t to_way;
};
```

All IDs in `OSMTurnRestriction` are OSM IDs and not local routing IDs. A restriction can be mandatory or prohibitive. A mandatory turn means that for this combination of `from_way` and `via_node` one may not switch onto another way than the `to_way`. If there are two mandatory turns with the same `from_way` and `via_node` but different `to_way`, `from_way` is a dead-end. A prohibitive turn means that it is not allowed to turn from `from_way` to `to_way`.

You may set `via_node` to `(uint64_t)-1`. In this case, RoutingKit will infer the `via_node`, if the corresponding ways only cross exactly once. Otherwise, the restriction is ignored. `osm_relation_id` is only used to log warnings. You can set it to any value, if you do not care about correct log messages.

# Standard Interpretations

The interface described in the previous section does not interpret any OSM tags. If you have specific needs, you have to write the callbacks that perform the interpretation yourself. Fortunately, for every common cases, RoutingKit provides out-of-the box functionality. These functions are not meant to be flexible or parametrizable. They purely exist to extract a reasonably good routing graph without much code. All functions are declared in `<routingkit/osm_profile.h>`.

## Car Interpretation

To extract a car routing graph, RoutingKit provides the following functions:

```cpp
bool is_osm_way_used_by_cars(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
unsigned get_osm_way_speed(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
std::string get_osm_way_name(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
OSMWayDirectionCategory get_osm_car_direction_category(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
void decode_osm_car_turn_restrictions(uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags, std::function<void(OSMTurnRestriction)>on_new_turn_restriction, std::function<void(const std::string&)>log_message = nullptr);
```

The function `is_osm_way_used_by_cars` is supposed to be used when determining the IDs in the first step. The other four functions should be used in the second extraction step.

`get_osm_way_speed` returns the speed in km/h of the corresponding way. `get_osm_way_name` returns the name of the way. If there is no name, the string is empty. `get_osm_car_direction_category` determines in what direction a car can traverse the way. All three functions should be called from within the `oneway_classifier`.

Finally, `decode_osm_car_turn_restrictions` is supposed to be used from within the `turn_restriction_classifier`.

## Bicycle Interpretation

To extract a bicycle routing graph, RoutingKit provides the following functions:

```cpp
bool is_osm_way_used_by_bicycles(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
unsigned char get_osm_way_bicycle_comfort_level(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
OSMWayDirectionCategory get_osm_bicycle_direction_category(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
unsigned char get_min_bicycle_comfort_level();
unsigned char get_max_bicycle_comfort_level();
```

`is_osm_way_used_by_bicycles` and `get_osm_bicycle_direction_category` work completely analogous to the car routing functions. The remaining three functions determine the comfort level of a way. For example, a primary road without bicycle lane is less comfortable than a way dedicated to only bicycles. The comfort level is a value between `get_min_bicycle_comfort_level()` and `get_max_bicycle_comfort_level()`. The later two functions return constants. However, these might change with newer RoutingKit versions and should therefore not be hard-coded.

`get_osm_way_name` can be used in exactly the same way as for extracting a car routing graph.

## Pedestrians Interpretation

To extract a pedestrian routing graph, RoutingKit only provides a single function, namely:

```cpp
bool is_osm_way_used_by_pedestrians(uint64_t osm_way_id, const TagMap&tags, std::function<void(const std::string&)>log_message = nullptr);
```

It is used analogously to `is_osm_way_used_by_bicycles` and `is_osm_way_used_by_cars`.
