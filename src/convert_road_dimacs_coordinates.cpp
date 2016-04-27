#include <routingkit/vector_io.h>
#include <routingkit/permutation.h>
#include <routingkit/sort.h>
#include <routingkit/inverse_vector.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	try{
		string dimacs_coord_file;
		string latitude_file;
		string longitude_file;

		if(argc != 4){
			cerr << argv[0] << " dimacs_coord_file latitude_file longitude_file" << endl;
			return 1;
		}else{
			dimacs_coord_file = argv[1];
			latitude_file = argv[2];
			longitude_file = argv[3];
		}

		cout << "Loading data ... " << flush;

		ifstream in(dimacs_coord_file);
		if(!in)
			throw runtime_error("Can not open \""+dimacs_coord_file+"\"");

		string line;
		unsigned line_num = 0;

		vector<float>latitude, longitude;
		unsigned node_count;

		bool was_header_read = false;
		std::vector<bool>seen;
		const double dimacs_scale = 1000000.0;

		while(std::getline(in, line)){
			++line_num;
			if(line.empty() || line[0] == 'c')
				continue;

			std::istringstream lin(line);
			if(!was_header_read){
				was_header_read = true;
				std::string b;
				if(!(lin>>b) || b != "p" || !(lin>>b) || b != "aux" || !(lin>>b) || b != "sp" || !(lin>>b) || b != "co")
					throw std::runtime_error("Header broken in line "+std::to_string(line_num));

				if(!(lin>>node_count))
					throw std::runtime_error("Header missing size");
				if(lin >> b)
					throw std::runtime_error("The header in line "+std::to_string(line_num)+" contains extra charachters at the end of the line");

				latitude.resize(node_count);
				longitude.resize(node_count);
				seen.resize(node_count,false);
			}else{
				std::string v;
				if(!(lin>>v) || v != "v")
					throw std::runtime_error("Line "+std::to_string(line_num)+" is broken");

				unsigned node;
				long long lon, lat;
				if(!(lin >> node >> lon >> lat))
					throw std::runtime_error("Could not read the data in line "+std::to_string(line_num));
			
				if(lin >> v)
					throw std::runtime_error("Line "+std::to_string(line_num)+" contains extra charachters at the end of the line");
				--node;
				if(node >= node_count)
					throw std::runtime_error("Node "+std::to_string(node)+" is out of bounds");
				if(seen[node])
					throw std::runtime_error("Node "+std::to_string(node)+" has a second pair of coordinates in line "+std::to_string(line_num));
				seen[node] = true;
				longitude[node] = static_cast<double>(lon) / dimacs_scale;
				latitude[node] = static_cast<double>(lat) / dimacs_scale;
			}
		}

		for(unsigned i=0; i<node_count; ++i)
			if(!seen[i])
				throw runtime_error("Position for node "+std::to_string(i)+" is missing");
		
		cout << "done" << endl;

		cout << "Saving file ... " << flush;
		save_vector(longitude_file, longitude);
		save_vector(latitude_file, latitude);
		cout << "done" << endl;

	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}
