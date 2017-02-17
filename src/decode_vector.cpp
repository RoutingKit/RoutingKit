#include <routingkit/vector_io.h>

#include <string>
#include <iostream>
#include <limits>
#include <iomanip>

using namespace RoutingKit;
using namespace std;

template<class T>
void convert_num_data(const string&input_vector_file){
	vector<T>v = load_vector<T>(input_vector_file);

	for(auto&x:v)
		cout << std::setprecision(std::numeric_limits<T>::digits10+1) << x << '\n';
}

string replace_all_substrings(string subject, const string& search, const string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return std::move(subject);
}

void convert_string_data(const string&input_vector_file){
	vector<string>v = load_vector<string>(input_vector_file);

	for(auto&s:v)
		cout << replace_all_substrings(replace_all_substrings(s, "\\", "\\\\"), "\n", "\\n") << '\n';
}

int main(int argc, char*argv[]){
	try{
		string data_type, input_vector_file;
		if(argc != 3){
			cerr
				<< "Usage: "<< argv[0] << " data_type input_vector_file\n"
				<< "\n"
				<< "Reads binary data from input_vector_file and writes the data to the standard output. data_type can be one of\n"
				<< " * int8\n"
				<< " * uint8\n"
				<< " * int16\n"
				<< " * uint16\n"
				<< " * int32\n"
				<< " * uint32\n"
				<< " * int64\n"
				<< " * uint64\n"
				<< " * float32\n"
				<< " * float64\n"
				<< " * string" << endl;
			return 1;
		}else{
			data_type = argv[1];
			input_vector_file = argv[2];
		}

		if(data_type == "int8")
			convert_num_data<signed char>(input_vector_file);
		else if(data_type == "uint8")
			convert_num_data<unsigned char>(input_vector_file);
		else if(data_type == "int16")
			convert_num_data<signed short>(input_vector_file);
		else if(data_type == "uint16")
			convert_num_data<unsigned short>(input_vector_file);
		else if(data_type == "int32")
			convert_num_data<signed int>(input_vector_file);
		else if(data_type == "uint32")
			convert_num_data<unsigned int>(input_vector_file);
		else if(data_type == "int64")
			convert_num_data<signed long long>(input_vector_file);
		else if(data_type == "uint64")
			convert_num_data<unsigned long long>(input_vector_file);
		else if(data_type == "float32")
			convert_num_data<float>(input_vector_file);
		else if(data_type == "float64")
			convert_num_data<double>(input_vector_file);
		else if(data_type == "string")
			convert_string_data(input_vector_file);
		else
			throw runtime_error("Unknown data type \""+data_type+"\"");
	}catch(exception&err){
		cerr << "Stopped on exception : "<< err.what() << endl;
	}
}
