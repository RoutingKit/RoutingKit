#include "expect.h"
#include <cmath>
#include <routingkit/google_polyline.h>
#include <vector>

using namespace RoutingKit;

namespace {

struct Point {
  float latitude;
  float longitude;
};

void run_encoding_test(const std::vector<Point> &polyline,
                       const std::string &correct_str) {
  GooglePolylineEncoder encoder;
  std::string computed_str;

  for (Point p : polyline) {
    encoder.encode(p.latitude, p.longitude, computed_str);
  }
  EXPECT_CMP(correct_str, ==, computed_str);
}

void run_decoding_test(const std::vector<Point> &correct_polyline,
                       const std::string &str) {
  GooglePolylineDecoder decoder;
  std::string_view remaining = str;
  unsigned point_index = 0;

  while (!remaining.empty()) {
    EXPECT_CMP(point_index, <, correct_polyline.size());
    GooglePolylineDecoder::Result result = decoder.decode(remaining);

    EXPECT_CMP(
        std::fabs(result.latitude - correct_polyline[point_index].latitude), <,
        0.00001);
    EXPECT_CMP(
        std::fabs(result.longitude - correct_polyline[point_index].longitude),
        <, 0.00001);

    remaining = result.remaining;
    ++point_index;
  }
  EXPECT_CMP(point_index, ==, correct_polyline.size());
}

void run_test(const std::vector<Point> &polyline, const std::string &str) {
  run_encoding_test(polyline, str);
  run_decoding_test(polyline, str);
}

float round_to_three_decimals(float x) {
  return std::round(x * 1000.0) / 1000.0;
}

std::vector<Point> make_test_circle() {
  std::vector<Point> polyline;

  const float r = 50.f;
  float a = 0.f;
  for (int i = 0; i < 10000; ++i) {
    a += 0.01f;
    polyline.push_back({round_to_three_decimals(std::cos(a) * r),
                        round_to_three_decimals(std::sin(a) * r)});
  }
  return polyline;
}

void run_roundtrip_test(const std::vector<Point> &polyline) {
  GooglePolylineEncoder encoder;
  GooglePolylineDecoder decoder;
  std::string str;

  for (Point p : polyline) {
    str.clear();
    encoder.encode(p.latitude, p.longitude, str);
    GooglePolylineDecoder::Result result = decoder.decode(str);
    EXPECT_CMP(fabs(result.latitude - p.latitude), <=, 0.000015f);
    EXPECT_CMP(fabs(result.longitude - p.longitude), <=, 0.000015f);
    EXPECT(result.remaining.empty());
  }
}

} // namespace

int main() {
  // example from
  // https://developers.google.com/maps/documentation/utilities/polylinealgorithm
  run_test({{38.5, -120.2}}, "_p~iF~ps|U");
  run_test({{38.5, -120.2}, {40.7, -120.95}, {43.252, -126.453}},
           "_p~iF~ps|U_ulLnnqC_mqNvxq`@");

  run_test({{50, 0}, {50, 1}}, "_sdpH??_ibE");

  run_roundtrip_test(make_test_circle());

  return expect_failed;
}
