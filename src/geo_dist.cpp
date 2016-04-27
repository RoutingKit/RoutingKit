#include <routingkit/geo_dist.h>

#include <math.h>

namespace RoutingKit{

double geo_dist(double a_lat, double a_lon, double b_lat, double b_lon){

	const double pi = 3.14159265359;
	const double R = 6371000.0; // earth radius in meter

	a_lat /= 180;
	a_lat *= pi;
	b_lat /= 180;
	b_lat *= pi;
	a_lon /= 180;
	a_lon *= pi;
	b_lon /= 180;
	b_lon *= pi;
	
	double dlat = b_lat - a_lat;
	double dlon = b_lon - a_lon;

	double a_ = sin(dlat/2.0) * sin(dlat/2.0) + sin(dlon/2.0) * sin(dlon/2.0) * cos(a_lat) * cos(b_lat);
	double c = 2 * atan2(sqrt(a_), sqrt(1-a_));
	return R * c;
}

} // RoutingKit
