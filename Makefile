# This makefile was automatically generated. Run ./generate_make_file to regenerate the file.
CC=g++
AR=ar
CFLAGS=-Wall -O3 -DNDEBUG -mpopcnt -mavx -std=c++11 -fPIC -Iinclude
LDFLAGS=

all: bin/test_contraction_hierarchy_path_query bin/test_customizable_contraction_hierarchy_customization bin/test_customizable_contraction_hierarchy bin/encode_vector bin/compute_geographic_distance_weights bin/convert_road_dimacs_graph bin/test_customizable_contraction_hierarchy_pinned_query bin/test_inverse_vector bin/compute_nested_dissection_order bin/compare_vector bin/export_road_dimacs_graph bin/graph_to_svg bin/examine_ch bin/decode_vector bin/generate_random_node_list bin/test_buffered_asynchronous_reader bin/test_customizable_contraction_hierarchy_path_query bin/test_nested_dissection bin/test_sort bin/test_basic_features bin/osm_extract bin/convert_road_dimacs_coordinates bin/randomly_permute_nodes bin/generate_dijkstra_rank_test_queries bin/run_contraction_hierarchy_query bin/generate_constant_vector bin/graph_to_dot bin/run_dijkstra bin/test_tag_map bin/test_id_set_queue bin/test_contraction_hierarchy_pinned_query bin/test_bit_vector bin/show_path bin/compute_contraction_hierarchy bin/generate_random_source_times bin/test_id_mapper bin/test_customizable_contraction_hierarchy_perfect_customization bin/generate_test_queries bin/test_permutation lib/libroutingkit.a lib/libroutingkit.so

build/test_contraction_hierarchy_path_query.o: include/routingkit/id_queue.h include/routingkit/permutation.h include/routingkit/contraction_hierarchy.h include/routingkit/sort.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/timestamp_flag.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h src/test_contraction_hierarchy_path_query.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_contraction_hierarchy_path_query.cpp -o build/test_contraction_hierarchy_path_query.o

build/test_customizable_contraction_hierarchy_customization.o: include/routingkit/id_mapper.h include/routingkit/graph_util.h include/routingkit/permutation.h src/test_customizable_contraction_hierarchy_customization.cpp include/routingkit/sort.h include/routingkit/id_set_queue.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h include/routingkit/customizable_contraction_hierarchy.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_customizable_contraction_hierarchy_customization.cpp -o build/test_customizable_contraction_hierarchy_customization.o

build/test_customizable_contraction_hierarchy.o: include/routingkit/id_mapper.h include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/id_set_queue.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h include/routingkit/customizable_contraction_hierarchy.h src/test_customizable_contraction_hierarchy.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_customizable_contraction_hierarchy.cpp -o build/test_customizable_contraction_hierarchy.o

build/encode_vector.o: include/routingkit/bit_vector.h include/routingkit/vector_io.h src/encode_vector.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/encode_vector.cpp -o build/encode_vector.o

build/compute_geographic_distance_weights.o: src/compute_geographic_distance_weights.cpp include/routingkit/id_queue.h src/verify.h include/routingkit/geo_dist.h include/routingkit/vector_io.h include/routingkit/constants.h src/dijkstra.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/timestamp_flag.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/compute_geographic_distance_weights.cpp -o build/compute_geographic_distance_weights.o

build/convert_road_dimacs_graph.o: src/convert_road_dimacs_graph.cpp include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/convert_road_dimacs_graph.cpp -o build/convert_road_dimacs_graph.o

build/test_customizable_contraction_hierarchy_pinned_query.o: include/routingkit/id_mapper.h include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/id_set_queue.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h src/test_customizable_contraction_hierarchy_pinned_query.cpp include/routingkit/customizable_contraction_hierarchy.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_customizable_contraction_hierarchy_pinned_query.cpp -o build/test_customizable_contraction_hierarchy_pinned_query.o

build/buffered_asynchronous_reader.o: src/buffered_asynchronous_reader.cpp src/buffered_asynchronous_reader.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/buffered_asynchronous_reader.cpp -o build/buffered_asynchronous_reader.o

build/test_inverse_vector.o: src/expect.h include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/constants.h src/test_inverse_vector.cpp include/routingkit/min_max.h include/routingkit/inverse_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_inverse_vector.cpp -o build/test_inverse_vector.o

build/osm_simple.o: include/routingkit/permutation.h include/routingkit/osm_profile.h include/routingkit/osm_graph_builder.h include/routingkit/sort.h src/osm_simple.cpp include/routingkit/constants.h include/routingkit/osm_simple.h include/routingkit/min_max.h include/routingkit/bit_vector.h include/routingkit/inverse_vector.h include/routingkit/tag_map.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/osm_simple.cpp -o build/osm_simple.o

build/vector_io.o: include/routingkit/bit_vector.h include/routingkit/vector_io.h src/vector_io.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/vector_io.cpp -o build/vector_io.o

build/compute_nested_dissection_order.o: src/compute_nested_dissection_order.cpp include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h include/routingkit/nested_dissection.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/compute_nested_dissection_order.cpp -o build/compute_nested_dissection_order.o

build/compare_vector.o: include/routingkit/bit_vector.h src/compare_vector.cpp include/routingkit/vector_io.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/compare_vector.cpp -o build/compare_vector.o

build/export_road_dimacs_graph.o: src/export_road_dimacs_graph.cpp include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/export_road_dimacs_graph.cpp -o build/export_road_dimacs_graph.o

build/timer.o: include/routingkit/timer.h src/timer.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/timer.cpp -o build/timer.o

build/graph_to_svg.o: include/routingkit/id_queue.h src/graph_to_svg.cpp include/routingkit/contraction_hierarchy.h include/routingkit/permutation.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/timestamp_flag.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/graph_to_svg.cpp -o build/graph_to_svg.o

build/examine_ch.o: include/routingkit/permutation.h src/verify.h include/routingkit/id_queue.h include/routingkit/contraction_hierarchy.h include/routingkit/sort.h include/routingkit/vector_io.h include/routingkit/constants.h src/dijkstra.h src/examine_ch.cpp include/routingkit/timer.h include/routingkit/timestamp_flag.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/examine_ch.cpp -o build/examine_ch.o

build/bit_select.o: src/bit_select.h src/bit_select.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/bit_select.cpp -o build/bit_select.o

build/decode_vector.o: include/routingkit/bit_vector.h include/routingkit/vector_io.h src/decode_vector.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/decode_vector.cpp -o build/decode_vector.o

build/generate_random_node_list.o: include/routingkit/id_queue.h src/generate_random_node_list.cpp include/routingkit/vector_io.h include/routingkit/constants.h src/dijkstra.h include/routingkit/timestamp_flag.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/generate_random_node_list.cpp -o build/generate_random_node_list.o

build/contraction_hierarchy.o: include/routingkit/graph_util.h include/routingkit/permutation.h include/routingkit/id_queue.h include/routingkit/contraction_hierarchy.h include/routingkit/sort.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/timestamp_flag.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h src/contraction_hierarchy.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/contraction_hierarchy.cpp -o build/contraction_hierarchy.o

build/test_buffered_asynchronous_reader.o: src/buffered_asynchronous_reader.h src/test_buffered_asynchronous_reader.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_buffered_asynchronous_reader.cpp -o build/test_buffered_asynchronous_reader.o

build/test_customizable_contraction_hierarchy_path_query.o: include/routingkit/id_mapper.h include/routingkit/graph_util.h include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/id_set_queue.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h src/test_customizable_contraction_hierarchy_path_query.cpp include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h include/routingkit/customizable_contraction_hierarchy.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_customizable_contraction_hierarchy_path_query.cpp -o build/test_customizable_contraction_hierarchy_path_query.o

build/test_nested_dissection.o: src/expect.h include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/constants.h src/test_nested_dissection.cpp include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h include/routingkit/nested_dissection.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_nested_dissection.cpp -o build/test_nested_dissection.o

build/id_mapper.o: src/id_mapper.cpp src/bit_select.h include/routingkit/id_mapper.h include/routingkit/constants.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/id_mapper.cpp -o build/id_mapper.o

build/test_sort.o: src/expect.h src/test_sort.cpp include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/constants.h include/routingkit/timer.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_sort.cpp -o build/test_sort.o

build/file_data_source.o: src/file_data_source.cpp src/file_data_source.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/file_data_source.cpp -o build/file_data_source.o

build/vantage_point_tree.o: include/routingkit/geo_dist.h include/routingkit/vantage_point_tree.h include/routingkit/constants.h src/vantage_point_tree.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/vantage_point_tree.cpp -o build/vantage_point_tree.o

build/test_basic_features.o: src/expect.h include/routingkit/id_mapper.h include/routingkit/permutation.h include/routingkit/osm_profile.h include/routingkit/contraction_hierarchy.h include/routingkit/osm_graph_builder.h include/routingkit/id_queue.h include/routingkit/id_set_queue.h include/routingkit/sort.h include/routingkit/constants.h src/test_basic_features.cpp include/routingkit/timestamp_flag.h include/routingkit/min_max.h include/routingkit/bit_vector.h include/routingkit/inverse_vector.h include/routingkit/nested_dissection.h include/routingkit/customizable_contraction_hierarchy.h include/routingkit/tag_map.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_basic_features.cpp -o build/test_basic_features.o

build/osm_extract.o: src/osm_extract.cpp include/routingkit/permutation.h include/routingkit/osm_profile.h include/routingkit/osm_graph_builder.h include/routingkit/sort.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/bit_vector.h include/routingkit/inverse_vector.h include/routingkit/tag_map.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/osm_extract.cpp -o build/osm_extract.o

build/convert_road_dimacs_coordinates.o: include/routingkit/permutation.h include/routingkit/sort.h src/convert_road_dimacs_coordinates.cpp include/routingkit/constants.h include/routingkit/vector_io.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/convert_road_dimacs_coordinates.cpp -o build/convert_road_dimacs_coordinates.o

build/osm_pbf_decoder.o: src/osm_pbf_decoder.cpp src/buffered_asynchronous_reader.h include/routingkit/permutation.h include/routingkit/sort.h src/file_data_source.h include/routingkit/constants.h include/routingkit/timer.h src/osmpbfformat.pb.h src/osm_pbf_decoder.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/tag_map.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/osm_pbf_decoder.cpp -o build/osm_pbf_decoder.o

build/randomly_permute_nodes.o: src/randomly_permute_nodes.cpp include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/randomly_permute_nodes.cpp -o build/randomly_permute_nodes.o

build/generate_dijkstra_rank_test_queries.o: src/generate_dijkstra_rank_test_queries.cpp include/routingkit/id_queue.h src/verify.h include/routingkit/vector_io.h include/routingkit/constants.h src/dijkstra.h include/routingkit/timestamp_flag.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/generate_dijkstra_rank_test_queries.cpp -o build/generate_dijkstra_rank_test_queries.o

build/run_contraction_hierarchy_query.o: src/run_contraction_hierarchy_query.cpp include/routingkit/id_queue.h include/routingkit/permutation.h include/routingkit/contraction_hierarchy.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/timestamp_flag.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/run_contraction_hierarchy_query.cpp -o build/run_contraction_hierarchy_query.o

build/generate_constant_vector.o: include/routingkit/id_queue.h include/routingkit/vector_io.h include/routingkit/constants.h src/dijkstra.h include/routingkit/timestamp_flag.h include/routingkit/bit_vector.h src/generate_constant_vector.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/generate_constant_vector.cpp -o build/generate_constant_vector.o

build/geo_dist.o: include/routingkit/geo_dist.h src/geo_dist.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/geo_dist.cpp -o build/geo_dist.o

build/graph_to_dot.o: include/routingkit/id_queue.h include/routingkit/permutation.h include/routingkit/contraction_hierarchy.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/timestamp_flag.h src/graph_to_dot.cpp include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/graph_to_dot.cpp -o build/graph_to_dot.o

build/graph_util.o: include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/graph_util.h include/routingkit/constants.h src/graph_util.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/graph_util.cpp -o build/graph_util.o

build/run_dijkstra.o: include/routingkit/id_queue.h include/routingkit/vector_io.h include/routingkit/constants.h src/dijkstra.h include/routingkit/timer.h include/routingkit/min_max.h src/run_dijkstra.cpp include/routingkit/timestamp_flag.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/run_dijkstra.cpp -o build/run_dijkstra.o

build/nested_dissection.o: include/routingkit/id_mapper.h include/routingkit/graph_util.h include/routingkit/filter.h include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/inverse_vector.h src/nested_dissection.cpp include/routingkit/bit_vector.h include/routingkit/nested_dissection.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/nested_dissection.cpp -o build/nested_dissection.o

build/osm_graph_builder.o: include/routingkit/id_mapper.h src/osm_graph_builder.cpp include/routingkit/graph_util.h include/routingkit/filter.h include/routingkit/permutation.h include/routingkit/geo_dist.h include/routingkit/osm_graph_builder.h include/routingkit/sort.h include/routingkit/constants.h include/routingkit/timer.h src/osm_pbf_decoder.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h include/routingkit/tag_map.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/osm_graph_builder.cpp -o build/osm_graph_builder.o

build/test_tag_map.o: src/expect.h include/routingkit/permutation.h src/test_tag_map.cpp include/routingkit/sort.h include/routingkit/constants.h include/routingkit/min_max.h include/routingkit/inverse_vector.h include/routingkit/tag_map.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_tag_map.cpp -o build/test_tag_map.o

build/bit_vector.o: include/routingkit/bit_vector.h src/bit_vector.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/bit_vector.cpp -o build/bit_vector.o

build/verify.o: src/verify.cpp src/verify.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/verify.cpp -o build/verify.o

build/osm_profile.o: include/routingkit/permutation.h include/routingkit/osm_profile.h include/routingkit/osm_graph_builder.h src/osm_profile.cpp include/routingkit/sort.h include/routingkit/constants.h include/routingkit/min_max.h include/routingkit/bit_vector.h include/routingkit/inverse_vector.h include/routingkit/tag_map.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/osm_profile.cpp -o build/osm_profile.o

build/customizable_contraction_hierarchy.o: include/routingkit/id_mapper.h include/routingkit/graph_util.h include/routingkit/filter.h include/routingkit/id_queue.h include/routingkit/permutation.h include/routingkit/contraction_hierarchy.h include/routingkit/sort.h src/customizable_contraction_hierarchy.cpp include/routingkit/id_set_queue.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/timestamp_flag.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h include/routingkit/customizable_contraction_hierarchy.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS) -fopenmp -c src/customizable_contraction_hierarchy.cpp -o build/customizable_contraction_hierarchy.o

build/osmpbfformat.pb.o: src/osmpbfformat.pb.cpp src/osmpbfformat.pb.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/osmpbfformat.pb.cpp -o build/osmpbfformat.pb.o

build/test_id_set_queue.o: src/test_id_set_queue.cpp src/expect.h include/routingkit/id_set_queue.h include/routingkit/min_max.h include/routingkit/constants.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_id_set_queue.cpp -o build/test_id_set_queue.o

build/test_contraction_hierarchy_pinned_query.o: src/test_contraction_hierarchy_pinned_query.cpp include/routingkit/id_queue.h include/routingkit/permutation.h include/routingkit/contraction_hierarchy.h include/routingkit/sort.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/timestamp_flag.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_contraction_hierarchy_pinned_query.cpp -o build/test_contraction_hierarchy_pinned_query.o

build/test_bit_vector.o: src/expect.h include/routingkit/bit_vector.h src/test_bit_vector.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_bit_vector.cpp -o build/test_bit_vector.o

build/show_path.o: include/routingkit/id_queue.h include/routingkit/permutation.h include/routingkit/contraction_hierarchy.h src/show_path.cpp include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/timestamp_flag.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/show_path.cpp -o build/show_path.o

build/compute_contraction_hierarchy.o: include/routingkit/id_queue.h src/verify.h include/routingkit/contraction_hierarchy.h include/routingkit/permutation.h include/routingkit/sort.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/timestamp_flag.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h src/compute_contraction_hierarchy.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/compute_contraction_hierarchy.cpp -o build/compute_contraction_hierarchy.o

build/generate_random_source_times.o: include/routingkit/id_queue.h include/routingkit/vector_io.h include/routingkit/constants.h src/dijkstra.h src/generate_random_source_times.cpp include/routingkit/timestamp_flag.h include/routingkit/bit_vector.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/generate_random_source_times.cpp -o build/generate_random_source_times.o

build/test_id_mapper.o: src/test_id_mapper.cpp src/expect.h include/routingkit/id_mapper.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/bit_vector.h src/bit_select.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_id_mapper.cpp -o build/test_id_mapper.o

build/test_customizable_contraction_hierarchy_perfect_customization.o: include/routingkit/id_mapper.h include/routingkit/graph_util.h include/routingkit/id_queue.h include/routingkit/permutation.h src/test_customizable_contraction_hierarchy_perfect_customization.cpp include/routingkit/contraction_hierarchy.h include/routingkit/sort.h include/routingkit/id_set_queue.h include/routingkit/vector_io.h include/routingkit/constants.h include/routingkit/timer.h include/routingkit/min_max.h include/routingkit/timestamp_flag.h include/routingkit/inverse_vector.h include/routingkit/bit_vector.h include/routingkit/customizable_contraction_hierarchy.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_customizable_contraction_hierarchy_perfect_customization.cpp -o build/test_customizable_contraction_hierarchy_perfect_customization.o

build/generate_test_queries.o: include/routingkit/bit_vector.h src/generate_test_queries.cpp include/routingkit/vector_io.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/generate_test_queries.cpp -o build/generate_test_queries.o

build/test_permutation.o: include/routingkit/permutation.h src/expect.h include/routingkit/constants.h src/test_permutation.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_permutation.cpp -o build/test_permutation.o

bin/test_contraction_hierarchy_path_query: build/test_contraction_hierarchy_path_query.o build/bit_vector.o build/graph_util.o build/contraction_hierarchy.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/test_contraction_hierarchy_path_query.o build/bit_vector.o build/graph_util.o build/contraction_hierarchy.o build/vector_io.o build/timer.o  -o bin/test_contraction_hierarchy_path_query

bin/test_customizable_contraction_hierarchy_customization: build/test_customizable_contraction_hierarchy_customization.o build/customizable_contraction_hierarchy.o build/bit_vector.o build/id_mapper.o build/graph_util.o build/contraction_hierarchy.o build/bit_select.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/test_customizable_contraction_hierarchy_customization.o build/customizable_contraction_hierarchy.o build/bit_vector.o build/id_mapper.o build/graph_util.o build/contraction_hierarchy.o build/bit_select.o build/vector_io.o build/timer.o -fopenmp  -o bin/test_customizable_contraction_hierarchy_customization

bin/test_customizable_contraction_hierarchy: build/timer.o build/customizable_contraction_hierarchy.o build/bit_vector.o build/id_mapper.o build/graph_util.o build/contraction_hierarchy.o build/bit_select.o build/vector_io.o build/test_customizable_contraction_hierarchy.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/timer.o build/customizable_contraction_hierarchy.o build/bit_vector.o build/id_mapper.o build/graph_util.o build/contraction_hierarchy.o build/bit_select.o build/vector_io.o build/test_customizable_contraction_hierarchy.o -fopenmp  -o bin/test_customizable_contraction_hierarchy

bin/encode_vector: build/bit_vector.o build/encode_vector.o build/vector_io.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/bit_vector.o build/encode_vector.o build/vector_io.o  -o bin/encode_vector

bin/compute_geographic_distance_weights: build/compute_geographic_distance_weights.o build/bit_vector.o build/verify.o build/geo_dist.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/compute_geographic_distance_weights.o build/bit_vector.o build/verify.o build/geo_dist.o build/vector_io.o build/timer.o -lm  -o bin/compute_geographic_distance_weights

bin/convert_road_dimacs_graph: build/convert_road_dimacs_graph.o build/bit_vector.o build/vector_io.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/convert_road_dimacs_graph.o build/bit_vector.o build/vector_io.o  -o bin/convert_road_dimacs_graph

bin/test_customizable_contraction_hierarchy_pinned_query: build/customizable_contraction_hierarchy.o build/bit_vector.o build/id_mapper.o build/graph_util.o build/contraction_hierarchy.o build/bit_select.o build/test_customizable_contraction_hierarchy_pinned_query.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/customizable_contraction_hierarchy.o build/bit_vector.o build/id_mapper.o build/graph_util.o build/contraction_hierarchy.o build/bit_select.o build/test_customizable_contraction_hierarchy_pinned_query.o build/vector_io.o build/timer.o -fopenmp  -o bin/test_customizable_contraction_hierarchy_pinned_query

bin/test_inverse_vector: build/test_inverse_vector.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/test_inverse_vector.o  -o bin/test_inverse_vector

bin/compute_nested_dissection_order: build/compute_nested_dissection_order.o build/bit_vector.o build/id_mapper.o build/graph_util.o build/nested_dissection.o build/bit_select.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/compute_nested_dissection_order.o build/bit_vector.o build/id_mapper.o build/graph_util.o build/nested_dissection.o build/bit_select.o build/vector_io.o build/timer.o  -o bin/compute_nested_dissection_order

bin/compare_vector: build/compare_vector.o build/bit_vector.o build/vector_io.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/compare_vector.o build/bit_vector.o build/vector_io.o  -o bin/compare_vector

bin/export_road_dimacs_graph: build/export_road_dimacs_graph.o build/bit_vector.o build/vector_io.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/export_road_dimacs_graph.o build/bit_vector.o build/vector_io.o  -o bin/export_road_dimacs_graph

bin/graph_to_svg: build/graph_to_svg.o build/bit_vector.o build/graph_util.o build/contraction_hierarchy.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/graph_to_svg.o build/bit_vector.o build/graph_util.o build/contraction_hierarchy.o build/vector_io.o build/timer.o  -o bin/graph_to_svg

bin/examine_ch: build/bit_vector.o build/examine_ch.o build/graph_util.o build/verify.o build/contraction_hierarchy.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/bit_vector.o build/examine_ch.o build/graph_util.o build/verify.o build/contraction_hierarchy.o build/vector_io.o build/timer.o  -o bin/examine_ch

bin/decode_vector: build/bit_vector.o build/vector_io.o build/decode_vector.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/bit_vector.o build/vector_io.o build/decode_vector.o  -o bin/decode_vector

bin/generate_random_node_list: build/generate_random_node_list.o build/bit_vector.o build/vector_io.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/generate_random_node_list.o build/bit_vector.o build/vector_io.o  -o bin/generate_random_node_list

bin/test_buffered_asynchronous_reader: build/buffered_asynchronous_reader.o build/test_buffered_asynchronous_reader.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/buffered_asynchronous_reader.o build/test_buffered_asynchronous_reader.o -pthread  -o bin/test_buffered_asynchronous_reader

bin/test_customizable_contraction_hierarchy_path_query: build/customizable_contraction_hierarchy.o build/bit_vector.o build/test_customizable_contraction_hierarchy_path_query.o build/id_mapper.o build/graph_util.o build/contraction_hierarchy.o build/bit_select.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/customizable_contraction_hierarchy.o build/bit_vector.o build/test_customizable_contraction_hierarchy_path_query.o build/id_mapper.o build/graph_util.o build/contraction_hierarchy.o build/bit_select.o build/vector_io.o build/timer.o -fopenmp  -o bin/test_customizable_contraction_hierarchy_path_query

bin/test_nested_dissection: build/bit_vector.o build/test_nested_dissection.o build/id_mapper.o build/graph_util.o build/nested_dissection.o build/bit_select.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/bit_vector.o build/test_nested_dissection.o build/id_mapper.o build/graph_util.o build/nested_dissection.o build/bit_select.o build/timer.o  -o bin/test_nested_dissection

bin/test_sort: build/test_sort.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/test_sort.o build/timer.o  -o bin/test_sort

bin/test_basic_features: build/timer.o build/osm_pbf_decoder.o build/osmpbfformat.pb.o build/osm_graph_builder.o build/osm_profile.o build/customizable_contraction_hierarchy.o build/bit_vector.o build/test_basic_features.o build/id_mapper.o build/file_data_source.o build/graph_util.o build/nested_dissection.o build/contraction_hierarchy.o build/bit_select.o build/geo_dist.o build/vector_io.o build/buffered_asynchronous_reader.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/timer.o build/osm_pbf_decoder.o build/osmpbfformat.pb.o build/osm_graph_builder.o build/osm_profile.o build/customizable_contraction_hierarchy.o build/bit_vector.o build/test_basic_features.o build/id_mapper.o build/file_data_source.o build/graph_util.o build/nested_dissection.o build/contraction_hierarchy.o build/bit_select.o build/geo_dist.o build/vector_io.o build/buffered_asynchronous_reader.o -pthread -fopenmp -lprotobuf -lm -lz  -o bin/test_basic_features

bin/osm_extract: build/osm_pbf_decoder.o build/osmpbfformat.pb.o build/osm_extract.o build/osm_graph_builder.o build/osm_profile.o build/bit_vector.o build/id_mapper.o build/file_data_source.o build/graph_util.o build/geo_dist.o build/buffered_asynchronous_reader.o build/bit_select.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/osm_pbf_decoder.o build/osmpbfformat.pb.o build/osm_extract.o build/osm_graph_builder.o build/osm_profile.o build/bit_vector.o build/id_mapper.o build/file_data_source.o build/graph_util.o build/geo_dist.o build/buffered_asynchronous_reader.o build/bit_select.o build/vector_io.o build/timer.o -pthread -lm -lprotobuf -lz  -o bin/osm_extract

bin/convert_road_dimacs_coordinates: build/convert_road_dimacs_coordinates.o build/bit_vector.o build/vector_io.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/convert_road_dimacs_coordinates.o build/bit_vector.o build/vector_io.o  -o bin/convert_road_dimacs_coordinates

bin/randomly_permute_nodes: build/bit_vector.o build/randomly_permute_nodes.o build/vector_io.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/bit_vector.o build/randomly_permute_nodes.o build/vector_io.o  -o bin/randomly_permute_nodes

bin/generate_dijkstra_rank_test_queries: build/verify.o build/generate_dijkstra_rank_test_queries.o build/bit_vector.o build/vector_io.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/verify.o build/generate_dijkstra_rank_test_queries.o build/bit_vector.o build/vector_io.o  -o bin/generate_dijkstra_rank_test_queries

bin/run_contraction_hierarchy_query: build/run_contraction_hierarchy_query.o build/bit_vector.o build/graph_util.o build/contraction_hierarchy.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/run_contraction_hierarchy_query.o build/bit_vector.o build/graph_util.o build/contraction_hierarchy.o build/vector_io.o build/timer.o  -o bin/run_contraction_hierarchy_query

bin/generate_constant_vector: build/bit_vector.o build/vector_io.o build/generate_constant_vector.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/bit_vector.o build/vector_io.o build/generate_constant_vector.o  -o bin/generate_constant_vector

bin/graph_to_dot: build/bit_vector.o build/graph_to_dot.o build/graph_util.o build/contraction_hierarchy.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/bit_vector.o build/graph_to_dot.o build/graph_util.o build/contraction_hierarchy.o build/vector_io.o build/timer.o  -o bin/graph_to_dot

bin/run_dijkstra: build/timer.o build/bit_vector.o build/run_dijkstra.o build/vector_io.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/timer.o build/bit_vector.o build/run_dijkstra.o build/vector_io.o  -o bin/run_dijkstra

bin/test_tag_map: build/test_tag_map.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/test_tag_map.o  -o bin/test_tag_map

bin/test_id_set_queue: build/test_id_set_queue.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/test_id_set_queue.o  -o bin/test_id_set_queue

bin/test_contraction_hierarchy_pinned_query: build/test_contraction_hierarchy_pinned_query.o build/bit_vector.o build/graph_util.o build/contraction_hierarchy.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/test_contraction_hierarchy_pinned_query.o build/bit_vector.o build/graph_util.o build/contraction_hierarchy.o build/vector_io.o build/timer.o  -o bin/test_contraction_hierarchy_pinned_query

bin/test_bit_vector: build/test_bit_vector.o build/bit_vector.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/test_bit_vector.o build/bit_vector.o  -o bin/test_bit_vector

bin/show_path: build/show_path.o build/bit_vector.o build/graph_util.o build/contraction_hierarchy.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/show_path.o build/bit_vector.o build/graph_util.o build/contraction_hierarchy.o build/vector_io.o build/timer.o  -o bin/show_path

bin/compute_contraction_hierarchy: build/bit_vector.o build/graph_util.o build/verify.o build/compute_contraction_hierarchy.o build/contraction_hierarchy.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/bit_vector.o build/graph_util.o build/verify.o build/compute_contraction_hierarchy.o build/contraction_hierarchy.o build/vector_io.o build/timer.o  -o bin/compute_contraction_hierarchy

bin/generate_random_source_times: build/generate_random_source_times.o build/bit_vector.o build/vector_io.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/generate_random_source_times.o build/bit_vector.o build/vector_io.o  -o bin/generate_random_source_times

bin/test_id_mapper: build/test_id_mapper.o build/id_mapper.o build/bit_select.o build/bit_vector.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/test_id_mapper.o build/id_mapper.o build/bit_select.o build/bit_vector.o build/timer.o  -o bin/test_id_mapper

bin/test_customizable_contraction_hierarchy_perfect_customization: build/test_customizable_contraction_hierarchy_perfect_customization.o build/customizable_contraction_hierarchy.o build/bit_vector.o build/id_mapper.o build/graph_util.o build/contraction_hierarchy.o build/bit_select.o build/vector_io.o build/timer.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/test_customizable_contraction_hierarchy_perfect_customization.o build/customizable_contraction_hierarchy.o build/bit_vector.o build/id_mapper.o build/graph_util.o build/contraction_hierarchy.o build/bit_select.o build/vector_io.o build/timer.o -fopenmp  -o bin/test_customizable_contraction_hierarchy_perfect_customization

bin/generate_test_queries: build/generate_test_queries.o build/bit_vector.o build/vector_io.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/generate_test_queries.o build/bit_vector.o build/vector_io.o  -o bin/generate_test_queries

bin/test_permutation: build/test_permutation.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/test_permutation.o  -o bin/test_permutation

lib/libroutingkit.a: build/vantage_point_tree.o build/osm_pbf_decoder.o build/osmpbfformat.pb.o build/bit_select.o build/osm_graph_builder.o build/osm_profile.o build/customizable_contraction_hierarchy.o build/osm_simple.o build/bit_vector.o build/id_mapper.o build/file_data_source.o build/graph_util.o build/nested_dissection.o build/contraction_hierarchy.o build/geo_dist.o build/buffered_asynchronous_reader.o build/vector_io.o build/timer.o
	mkdir -p lib
	$(AR) rcs lib/libroutingkit.a build/vantage_point_tree.o build/osm_pbf_decoder.o build/osmpbfformat.pb.o build/bit_select.o build/osm_graph_builder.o build/osm_profile.o build/customizable_contraction_hierarchy.o build/osm_simple.o build/bit_vector.o build/id_mapper.o build/file_data_source.o build/graph_util.o build/nested_dissection.o build/contraction_hierarchy.o build/geo_dist.o build/buffered_asynchronous_reader.o build/vector_io.o build/timer.o

lib/libroutingkit.so: build/vantage_point_tree.o build/osm_pbf_decoder.o build/osmpbfformat.pb.o build/bit_select.o build/osm_graph_builder.o build/osm_profile.o build/customizable_contraction_hierarchy.o build/osm_simple.o build/bit_vector.o build/id_mapper.o build/file_data_source.o build/graph_util.o build/nested_dissection.o build/contraction_hierarchy.o build/geo_dist.o build/buffered_asynchronous_reader.o build/vector_io.o build/timer.o
	mkdir -p lib
	$(CC) -shared $(LDFLAGS) build/vantage_point_tree.o build/osm_pbf_decoder.o build/osmpbfformat.pb.o build/bit_select.o build/osm_graph_builder.o build/osm_profile.o build/customizable_contraction_hierarchy.o build/osm_simple.o build/bit_vector.o build/id_mapper.o build/file_data_source.o build/graph_util.o build/nested_dissection.o build/contraction_hierarchy.o build/geo_dist.o build/buffered_asynchronous_reader.o build/vector_io.o build/timer.o -pthread -lm -lprotobuf -fopenmp -lz -o lib/libroutingkit.so

