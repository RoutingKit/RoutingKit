#include <routingkit/vector_io.h>
#include <routingkit/permutation.h>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/min_max.h>
#include <routingkit/timer.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <iomanip>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		string ch_file;
		string latitude_file;
		string longitude_file;

		if(argc != 4){
			cerr << argv[0] << " ch_file latitude longitude" << endl;
			return 1;
		}else{
			ch_file = argv[1];
			latitude_file = argv[2];
			longitude_file = argv[3];
		}

		ContractionHierarchy ch = ContractionHierarchy::load_file(ch_file);
		vector<float>latitude = load_vector<float>(latitude_file);
		vector<float>longitude = load_vector<float>(longitude_file);

		ContractionHierarchyQuery ch_query(ch);		

		unsigned source, target;
		while(cin >> source >> target){
			if(source > ch.node_count())
				throw runtime_error("Source out of bounds");
			if(target > ch.node_count())
				throw runtime_error("Target out of bounds");

			ch_query.reset().add_source(source).add_target(target).run();

			cout <<
				"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
				"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
		  		"<Document>\n"
				"\t<Placemark>\n"
				"\t\t<name>Source</name>\n"
				"\t\t<Point><coordinates>"<< std::setprecision(10) << longitude[source] << ',' << std::setprecision(10) << latitude[source] << ",0</coordinates></Point>\n"
				"\t</Placemark>\n"
				"\t<Placemark>\n"
				"\t\t<name>Target</name>\n"
				"\t\t<Point><coordinates>" << std::setprecision(10) << longitude[target] << ',' << std::setprecision(10) << latitude[target] << ",0</coordinates></Point>\n"
				"\t</Placemark>\n"
				"\t<Placemark>\n"
				"\t\t<name>Path of length " << ch_query.get_distance() << " meter</name>\n"
				"\t\t<LineString>\n"
				"\t\t\t<coordinates>\n";
			for(unsigned x:ch_query.get_node_path())
				cout << "\t\t\t\t" << std::setprecision(10) << longitude[x] << ',' << std::setprecision(10) << latitude[x] << ",0\n";
			cout <<
				"\t\t\t</coordinates>\n"
				"\t\t</LineString>\n"
				"\t</Placemark>\n"
				"</Document>\n"
				"</kml>" << endl;

		}
	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}

