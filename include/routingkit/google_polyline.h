#ifndef ROUTING_KIT_GOOGLE_POLYLINE_H
#define ROUTING_KIT_GOOGLE_POLYLINE_H
#include <string>
#include <string_view>

namespace RoutingKit {

/// A one dimensional variant of GooglePolylineEncoder.
/// This is most likely only useful to implement
/// GooglePolylineEncoder.
class GoogleOneDimensionalPolylineEncoder {
public:
  /// Appends the encoded float to str.
  void encode(float val, std::string &str);

private:
  int last_val = 0;
};

/// Encodes a polyline using the Google polyline format as described here:
/// https://developers.google.com/maps/documentation/utilities/polylinealgorithm
/// Calling encode yields the characters for the next point. It does not
/// give you the full string. To obtain the full string, loop over the
/// polyline points and concatenate the results.
class GooglePolylineEncoder {
public:
  /// Appends the encoded coordinates to str.
  void encode(float lat, float lon, std::string &str);

private:
  GoogleOneDimensionalPolylineEncoder lat_encoder;
  GoogleOneDimensionalPolylineEncoder lon_encoder;
};

/// A one dimensional variant of GooglePolylineDecoder.
/// This is most likely only useful to implement
/// GooglePolylineDecoder.
class GoogleOneDimensionalPolylineDecoder {
public:
  struct Result {
    float value;
    std::string_view remaining;
  };

  /// Consumes as many chars as necessary to form the next float and
  /// returns it in Result::value. The remaining chars from the
  /// string are returned in Result::remaining.
  Result decode(std::string_view str);

private:
  int last_val = 0;
};

/// Decodes a polyline in the Google polyline format.
/// It performs the inverse of GooglePolylineEncoder.
class GooglePolylineDecoder {
public:
  struct Result {
    float latitude;
    float longitude;
    std::string_view remaining;
  };

  /// Consumes as many chars as necessary to form the next coordinate
  /// and returns it in Result::value. The remaining chars from the
  /// string are returned in Result::remaining.
  Result decode(std::string_view str);

private:
  GoogleOneDimensionalPolylineDecoder lat_decoder;
  GoogleOneDimensionalPolylineDecoder lon_decoder;
};

} // namespace RoutingKit

#endif
