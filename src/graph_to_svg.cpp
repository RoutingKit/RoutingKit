#include <routingkit/vector_io.h>
#include <routingkit/timer.h>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/min_max.h>

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		string first_out_file;
		string head_file;
		string latitude_file;
		string longitude_file;
		string svg_file;
		
		if(argc != 6){
			cerr << argv[0] << " first_out_file head_file latitude_file longitude_file svg_file" << endl;
			return 1;
		}else{
			first_out_file = argv[1];
			head_file = argv[2];
			latitude_file = argv[3];
			longitude_file = argv[4];
			svg_file = argv[5];
		}

		cout << "Loading graph ... " << flush;

		vector<unsigned>first_out_arc = load_vector<unsigned>(first_out_file);
		vector<unsigned>head = load_vector<unsigned>(head_file);
		vector<float>latitude = load_vector<float>(latitude_file);
		vector<float>longitude = load_vector<float>(longitude_file);
		
		cout << "done" << endl;

		float min_latitude = min_element_of(latitude);
		float max_latitude = max_element_of(latitude);
		
		for(auto&x:latitude){
			x -= min_latitude;
			x *= 300;
			x /= (max_latitude - min_latitude);
		}

		float min_longitude = min_element_of(longitude);
		float max_longitude = max_element_of(longitude);

		for(auto&x:longitude){
			x -= min_longitude;
			x *= 300;
			x /= (max_longitude - min_longitude);
		}

		cout << "Writing SVG ... " << flush;

		std::ofstream out(svg_file);
		if(!out)
			throw std::runtime_error("Can not open file "+svg_file);

		out <<
			"<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"0 0 300 300\">\n";
		for(unsigned x=0; x<first_out_arc.size()-1; ++x){
			for(unsigned xy=first_out_arc[x]; xy<first_out_arc[x+1]; ++xy){
				unsigned y=head[xy];

				out << "<line "
					<< "x1=\"" << longitude[x] << "\" y1=\"" << 300-latitude[x] << "\" "
					<< "x2=\"" << longitude[y] << "\" y2=\""<< 300-latitude[y] <<"\" style=\"stroke:#000000;\"/>\n";
			}
		}
		out << "</svg>\n";

		cout << "done" << endl;

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

