#include <routingkit/google_polyline.h>

#include <iostream>
using namespace std;

namespace RoutingKit {

void GoogleOneDimensionalPolylineEncoder::encode(float val, std::string &str) {
  const int int_val = static_cast<int>(val * 100000);
  const int delta_val = int_val - last_val;
  last_val = int_val;

  unsigned uint_val = static_cast<unsigned>(delta_val);

  uint_val <<= 1;

  if (delta_val < 0) {
    uint_val = ~uint_val;
  }

  do {
    unsigned chunk = uint_val & ((1u << 5) - 1);

    uint_val >>= 5;
    if (uint_val != 0) {
      chunk |= 0x20;
    }
    chunk += 0x3f;

    str.push_back(chunk);
  } while (uint_val != 0);
}

void GooglePolylineEncoder::encode(float lat, float lon, std::string &str) {
  lat_encoder.encode(lat, str);
  lon_encoder.encode(lon, str);
}

GoogleOneDimensionalPolylineDecoder::Result
GoogleOneDimensionalPolylineDecoder::decode(std::string_view str) {

  unsigned uint_val = 0;
  unsigned str_pos = 0;
  bool has_next = true;
  while (has_next && str_pos < str.size()) {
    unsigned chunk = str[str_pos];
    chunk -= 0x3f;
    has_next = (chunk & 0x20);
    chunk &= ((1u << 5) - 1);

    uint_val |= chunk << (5 * str_pos);
    ++str_pos;
  }

  bool is_negative = uint_val & 1;

  uint_val >>= 1;
  if (is_negative) {
    uint_val = ~uint_val;
  }

  int int_val = static_cast<int>(uint_val);

  int_val += last_val;
  last_val = int_val;

  float val = static_cast<float>(int_val) / 100000.f;

  GoogleOneDimensionalPolylineDecoder::Result result;
  result.value = val;
  result.remaining = str.substr(str_pos);
  return result;
}

GooglePolylineDecoder::Result
GooglePolylineDecoder::decode(std::string_view str) {
  GoogleOneDimensionalPolylineDecoder::Result lat_result =
      lat_decoder.decode(str);
  GoogleOneDimensionalPolylineDecoder::Result lon_result =
      lon_decoder.decode(lat_result.remaining);
  GooglePolylineDecoder::Result result;
  result.latitude = lat_result.value;
  result.longitude = lon_result.value;
  result.remaining = lon_result.remaining;
  return result;
}

} // namespace RoutingKit
