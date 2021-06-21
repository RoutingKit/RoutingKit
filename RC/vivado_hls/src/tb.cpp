#include "geo_dist.hpp"
using namespace hls;

int main(){

	data_t lat_a = (data_t)rand();
	data_t lat_b = (data_t)rand();
	data_t lon_a = (data_t)rand();
	data_t lon_b = (data_t)rand();

	data_t result;
	data_t expected;

	data_t tolerance = (data_t)0.1;

	// Get expected result
	const data_t pi_div_180 = 3.14159265359/180.0;
	const data_t earth_radius = 6371000.785; // in meter

	data_t aux0 = hls::cos((lat_a - lat_b)*pi_div_180);
	data_t aux1 = hls::cos((lat_a + lat_b)*pi_div_180);
	data_t aux2 = hls::cos((lon_a - lon_b)*pi_div_180);
	expected  = (aux0 + aux1) * aux2 + aux0 - aux1;
	expected *= (data_t)0.5;
	expected  = hls::acos(expected);
	expected *= earth_radius;


	// Get actual result
	geo_dist(lat_a,lon_a,lat_b,lon_b,&result);

	// Compare results
	if(abs(result - expected) < tolerance){
		std::cout<<"The expected and actual results match! ("<<result<<")"<<std::endl;
		return 0;
	}
	else {
		std::cout<<"Mismatch! Result is "<<result<<" but expected is "<<expected<<std::endl;
		return 1;
	}

}
