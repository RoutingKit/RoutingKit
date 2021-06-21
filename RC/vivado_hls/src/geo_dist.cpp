#include "geo_dist.hpp"
using namespace hls;
void geo_dist(data_t lat_a, data_t lon_a, data_t lat_b, data_t lon_b, data_t* result){

#pragma HLS INTERFACE s_axilite register port=lat_a bundle=geo_dist_if
#pragma HLS INTERFACE s_axilite register port=lon_a bundle=geo_dist_if
#pragma HLS INTERFACE s_axilite register port=lat_b bundle=geo_dist_if
#pragma HLS INTERFACE s_axilite register port=lon_b bundle=geo_dist_if
#pragma HLS INTERFACE s_axilite register port=result bundle=geo_dist_if
#pragma HLS INTERFACE ap_ctrl_none port=return


	const data_t pi_div_180 = 3.14159265359/180.0;
	const data_t earth_radius = 6371000.785; // in meter

	data_t aux0 = hls::cos((lat_a - lat_b)*pi_div_180);
	data_t aux1 = hls::cos((lat_a + lat_b)*pi_div_180);
	data_t aux2 = hls::cos((lon_a - lon_b)*pi_div_180);
//	*result = 0.75;
	*result  = (aux0 + aux1) * aux2 + aux0 - aux1;
	*result *= (data_t)0.5;
	*result  = hls::acos(*result);
	*result *= earth_radius;
}
