#include <../src/expect.cpp>
//UNIT TESTS
#include <../google_test/test_buffered_asynchronous_reader.cpp>
#include <../google_test/test_contraction_hierarchy_extra_weight.cpp>
#include <../google_test/test_dijkstra.cpp>
#include <../google_test/test_customizable_contraction_hierarchy.cpp>
#include <../google_test/test_customizable_contraction_hierarchy_reset.cpp>
#include <../google_test/test_customizable_contraction_hierarchy_customization.cpp>
#include <../google_test/test_customizable_contraction_hierarchy_pinned_query.cpp>
#include <../google_test/test_geo_dist.cpp>
#include <../google_test/test_id_mapper.cpp>
#include <../google_test/test_id_set_queue.cpp>
#include <../google_test/test_inverse_vector.cpp>
#include <../google_test/test_nearest_neighbor.cpp>
#include <../google_test/test_nested_dissection.cpp>
#include <../google_test/test_permutation.cpp>
#include <../google_test/test_protobuf.cpp>
#include <../google_test/test_sort.cpp>
#include <../google_test/test_strongly_connected_component.cpp>
#include <../google_test/test_tag_map.cpp>
#include <../google_test/test_basic_features.cpp>
//INTEGRATION TEST
#include <../test_3.cpp>
//LIBRARIES
#include <iostream>
#include <gtest/gtest.h>

using namespace RoutingKit;
using namespace std;

TEST (google_test, Subtest_1) {
	char* argv[3];
	argv[1]="pitcairn.pbf";
	EXPECT_FALSE(test_basic_features(2, argv));
}

TEST (google_test, Subtest_2) {
	ASSERT_EQ(simple_load_osm_car_routing_graph_from_pbf("pitcairn.pbf").node_count(),103);
}


TEST (google_test, Subtest_3) {
	EXPECT_FALSE(test_nearest_neighbor());
}

TEST (google_test, Subtest_4) {
	EXPECT_FALSE(test_buffered_asynchronous_reader());
}

TEST (google_test, Subtest_5) {
	EXPECT_FALSE(test_geo_dist());
}

TEST (google_test, Subtest_6) {
	EXPECT_FALSE(test_id_mapper());
}

TEST (google_test, Subtest_7) {
	EXPECT_FALSE(test_id_queue());
}

TEST (google_test, Subtest_8) {
	EXPECT_FALSE(test_inverse_vector());
}

TEST (google_test, Subtest_9) {
	EXPECT_FALSE(test_nested_dissection());
}

TEST (google_test, Subtest_10) {
	EXPECT_FALSE(test_permutation());
}

TEST (google_test, Subtest_11) {
	EXPECT_FALSE(test_protobuf());
}

TEST (google_test, Subtest_12) {
	EXPECT_FALSE(test_sort());
}

TEST (google_test, Subtest_13) {
	EXPECT_FALSE(test_strongly_connected_component());
}

TEST (google_test, Subtest_14) {
	EXPECT_FALSE(test_tag_map());
}

TEST (google_test, Subtest_15) {
	char* argv[5];
	argv[1]="pitcairn.pbf";
	EXPECT_FALSE(test_contraction_hierarchy_extra_weight(1,argv));
}

TEST (google_test, Subtest_16) {
	char* argv[3];
	EXPECT_FALSE(test_customizable_contraction_hierarchy_pinned_query(5,argv));
}

TEST (google_test, Subtest_17) {
	char* argv[5];
	EXPECT_FALSE(test_dijkstra(4,argv));
}

TEST (google_test, Subtest_18) {
	char* argv[3];
	ASSERT_EQ(test_customizable_contraction_hierarchy(4,argv),0);
}

TEST (google_test, Subtest_19) {
	char* argv[3];
	EXPECT_FALSE(test_customizable_hierarchy_reset(4,argv));
}

TEST (google_test, Subtest_20) {
	char* argv[3];
	EXPECT_FALSE(test_customizable_contraction_hierarchy_customization(5,argv));
}

TEST (google_test, Integration_subtest) {
	testing::internal::CaptureStdout();
	pitcairn();
	string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output ,"The path is 25 71 70 88 91 81 86 5 19 87 41 58 33");
}

