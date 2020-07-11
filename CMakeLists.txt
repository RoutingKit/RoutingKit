cmake_minimum_required (VERSION 3.1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# By default, build in Release mode. Must appear before project() command
if (NOT DEFINED CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

project(RoutingKit)

# Bump versions on release
set(ROUTINGKIT_VERSION_MAJOR 2)
set(ROUTINGKIT_VERSION_MINOR 3)
set(ROUTINGKIT_VERSION_PATCH 2)
set(ROUTINGKIT_VERSION ${ROUTINGKIT_VERSION_MAJOR}.${ROUTINGKIT_VERSION_MINOR}.${ROUTINGKIT_VERSION_PATCH})

# Build options
option(CONFIG_ENABLE_EXAMPLES "Build the samples" ON)
option(BUILD_SHARED_LIBS "Build shared/static libs" ON)

# Dependencies
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

# Compiler options
if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
      string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

add_library(routingkit
    src/bit_select.cpp
    src/bit_vector.cpp
    src/buffered_asynchronous_reader.cpp
#    src/compare_vector.cpp
#    src/compute_contraction_hierarchy.cpp
#    src/compute_geographic_distance_weights.cpp
#    src/compute_nested_dissection_order.cpp
    src/contraction_hierarchy.cpp
#    src/convert_road_dimacs_coordinates.cpp
#    src/convert_road_dimacs_graph.cpp
    src/customizable_contraction_hierarchy.cpp
#    src/decode_vector.cpp
#    src/encode_vector.cpp
#    src/examine_ch.cpp
    src/expect.cpp
#    src/export_road_dimacs_graph.cpp
    src/file_data_source.cpp
#    src/generate_constant_vector.cpp
#    src/generate_dijkstra_rank_test_queries.cpp
#    src/generate_random_node_list.cpp
#    src/generate_random_source_times.cpp
    src/geo_position_to_node.cpp
#    src/graph_to_dot.cpp
#    src/graph_to_svg.cpp
    src/graph_util.cpp
    src/id_mapper.cpp
    src/nested_dissection.cpp
    src/osm_decoder.cpp
 #   src/osm_extract.cpp
    src/osm_graph_builder.cpp
    src/osm_profile.cpp
    src/osm_simple.cpp
    src/protobuf.cpp
#    src/randomly_permute_nodes.cpp
#    src/run_contraction_hierarchy_query.cpp
#    src/run_dijkstra.cpp
#    src/show_path.cpp
    src/strongly_connected_component.cpp
    src/timer.cpp
    src/vector_io.cpp
    src/verify.cpp

    src/bit_select.h
    src/buffered_asynchronous_reader.h
    src/emulate_gcc_builtin.h
    src/expect.h
    src/file_data_source.h
    src/protobuf.h
    src/verify.h
)
target_link_libraries(routingkit PRIVATE ZLIB::ZLIB Threads::Threads)

# use the headers in the build-tree or the installed ones
target_include_directories(routingkit PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# this compiles the "DLL" interface
target_compile_definitions(routingkit PRIVATE ROUTINGKIT_DLL)

if (BUILD_SHARED_LIBS)
  target_compile_definitions(routingkit PRIVATE ROUTINGKITLIB_EXPORTS)
else ()
  target_compile_definitions(routingkit PUBLIC ROUTINGKIT_STATIC)
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  target_compile_definitions(routingkit PRIVATE _DEBUG)
endif ()

set_target_properties(routingkit PROPERTIES
    VERSION ${ROUTINGKIT_VERSION}
    SOVERSION ${ROUTINGKIT_VERSION_MAJOR}
)

if(CONFIG_ENABLE_EXAMPLES)
  add_executable(example1 example/example1.cpp)
  target_link_libraries(example1 routingkit)
endif()

# The GNUInstallDirs defines ${CMAKE_INSTALL_DATAROOTDIR}
# See https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
include (GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/routingkit)

install(TARGETS routingkit
    EXPORT routingkit-export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT RuntimeLibraries
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT RuntimeLibraries
)

install(FILES
    include/routingkit/all.h
    include/routingkit/bit_vector.h
    include/routingkit/constants.h
    include/routingkit/contraction_hierarchy.h
    include/routingkit/customizable_contraction_hierarchy.h
    include/routingkit/dijkstra.h
    include/routingkit/filter.h
    include/routingkit/geo_dist.h
    include/routingkit/geo_position_to_node.h
    include/routingkit/graph_util.h
    include/routingkit/id_mapper.h
    include/routingkit/id_queue.h
    include/routingkit/id_set_queue.h
    include/routingkit/inverse_vector.h
    include/routingkit/min_max.h
    include/routingkit/nested_dissection.h
    include/routingkit/osm_decoder.h
    include/routingkit/osm_graph_builder.h
    include/routingkit/osm_profile.h
    include/routingkit/osm_simple.h
    include/routingkit/permutation.h
    include/routingkit/sort.h
    include/routingkit/strongly_connected_component.h
    include/routingkit/tag_map.h
    include/routingkit/timer.h
    include/routingkit/timestamp_flag.h
    include/routingkit/vector_io.h
    DESTINATION include/routingkit
    COMPONENT Development
)

# Export the target under the build-tree (no need to install)
export(EXPORT routingkit-export
    FILE "${CMAKE_BINARY_DIR}/routingkit-targets.cmake"
    NAMESPACE routingkit::
)

# Export the installed target (typically for packaging)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/routingkitConfigVersion.cmake"
    VERSION ${ROUTINGKIT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
configure_file(routingkitConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/routingkitConfig.cmake"
    COPYONLY
)
install(EXPORT routingkit-export
    FILE routingkit-targets.cmake
    NAMESPACE routingkit::
    DESTINATION ${INSTALL_CONFIGDIR}
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/routingkitConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/routingkitConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
    COMPONENT Development
)

# Define variables for the pkg-config file
set(PACKAGE_NAME routingkit)
configure_file(
    routingkit.pc.in
    ${CMAKE_BINARY_DIR}/routingkit.pc
    @ONLY
)
install(
    FILES ${CMAKE_BINARY_DIR}/routingkit.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

include(CTest)
enable_testing()

set(TEST_FILES
    src/test_basic_features.cpp
    src/test_bit_vector.cpp
    src/test_buffered_asynchronous_reader.cpp
    src/test_contraction_hierarchy_extra_weight.cpp
    src/test_contraction_hierarchy_path_query.cpp
    src/test_contraction_hierarchy_pinned_query.cpp
    src/test_customizable_contraction_hierarchy.cpp
    src/test_customizable_contraction_hierarchy_customization.cpp
    src/test_customizable_contraction_hierarchy_path_query.cpp
    src/test_customizable_contraction_hierarchy_perfect_customization.cpp
    src/test_customizable_contraction_hierarchy_pinned_query.cpp
    src/test_customizable_contraction_hierarchy_reset.cpp
    src/test_dijkstra.cpp
    src/test_geo_dist.cpp
    src/test_id_mapper.cpp
    src/test_id_set_queue.cpp
    src/test_inverse_vector.cpp
    src/test_nearest_neighbor.cpp
    src/test_nested_dissection.cpp
    src/test_osm_simple.cpp
    src/test_permutation.cpp
    src/test_protobuf.cpp
    src/test_sort.cpp
    src/test_strongly_connected_component.cpp
    src/test_tag_map.cpp
)

foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} routingkit)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

