cmake_minimum_required(VERSION 3.19..3.23)
if(${CMAKE_VERSION} VERSION_LESS 3.23)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.23)
endif()
project(RoutingKit VERSION 1.0
                  DESCRIPTION "RoutingKIT a CCH routing library"
                  LANGUAGES CXX)

add_library(routingkit 
    src/bit_select.cpp src/bit_vector.cpp src/buffered_asynchronous_reader.cpp src/contraction_hierarchy.cpp src/customizable_contraction_hierarchy.cpp src/file_data_source.cpp src/geo_position_to_node.cpp src/graph_util.cpp src/id_mapper.cpp src/nested_dissection.cpp src/osm_decoder.cpp src/osm_graph_builder.cpp src/osm_profile.cpp src/osm_simple.cpp src/protobuf.cpp src/strongly_connected_component.cpp src/timer.cpp src/vector_io.cpp  src/file_data_source.cpp
    src/file_data_source.h  src/protobuf.cpp src/protobuf.h src/buffered_asynchronous_reader.h 
    include/routingkit/bit_vector.h include/routingkit/constants.h include/routingkit/contraction_hierarchy.h include/routingkit/customizable_contraction_hierarchy.h include/routingkit/dijkstra.h include/routingkit/filter.h include/routingkit/geo_dist.h include/routingkit/geo_position_to_node.h include/routingkit/graph_util.h include/routingkit/id_mapper.h include/routingkit/id_queue.h include/routingkit/id_set_queue.h include/routingkit/inverse_vector.h include/routingkit/min_max.h include/routingkit/nested_dissection.h include/routingkit/osm_decoder.h include/routingkit/osm_graph_builder.h include/routingkit/osm_profile.h include/routingkit/osm_simple.h include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/strongly_connected_component.h include/routingkit/tag_map.h include/routingkit/timer.h include/routingkit/timestamp_flag.h include/routingkit/vector_io.h)
target_include_directories(routingkit PUBLIC include)
target_compile_features(routingkit PUBLIC cxx_std_11)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(routingkit PUBLIC OpenMP::OpenMP_CXX)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    enable_testing()
    set(ROUTINGKIT_TESTING_FILES src/test_sort.cpp src/expect.h src/expect.cpp src/verify.h src/verify.cpp)    
    set(ROUTINGKIT_TESTING_LIST test_osm_simple test_customizable_contraction_hierarchy_customization test_customizable_contraction_hierarchy_perfect_customization compute_contraction_hierarchy encode_vector test_contraction_hierarchy_path_query osm_extract compare_vector test_inverse_vector show_path test_strongly_connected_component convert_road_dimacs_coordinates test_protobuf test_customizable_contraction_hierarchy_pinned_query generate_random_node_list test_customizable_contraction_hierarchy_reset generate_dijkstra_rank_test_queries test_tag_map export_road_dimacs_graph test_contraction_hierarchy_extra_weight test_nearest_neighbor compute_nested_dissection_order test_permutation generate_test_queries randomly_permute_nodes graph_to_svg decode_vector test_sort convert_road_dimacs_graph test_customizable_contraction_hierarchy test_id_mapper test_contraction_hierarchy_pinned_query run_dijkstra test_dijkstra generate_constant_vector generate_random_source_times examine_ch compute_geographic_distance_weights graph_to_dot test_bit_vector run_contraction_hierarchy_query test_customizable_contraction_hierarchy_path_query test_id_set_queue test_nested_dissection test_geo_dist test_basic_features test_buffered_asynchronous_reader)
    foreach (TEST_ITEM ${ROUTINGKIT_TESTING_LIST}) 
        string(TOUPPER ${TEST_ITEM} TEST_NAME)
        add_executable(${TEST_ITEM} ${ROUTINGKIT_TESTING_FILES})
        target_link_libraries(${TEST_ITEM} PRIVATE routingkit)
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_ITEM})
    endforeach()
endif()