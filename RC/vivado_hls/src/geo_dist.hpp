
//#include <math.h>
#include "hls_math.h"
#include "ap_fixed.h"
#include <iostream>
#include <stdio.h>

typedef ap_fixed<20,8> data_t;

void geo_dist(data_t lat_a, data_t lon_a, data_t lat_b, data_t lon_b, data_t* result);
