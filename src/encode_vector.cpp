#include <routingkit/vector_io.h>

#include <string>
#include <iostream>
#include <limits>

using namespace RoutingKit;
using namespace std;


template<class T>
void convert_int_data(const string&output_vector_file){
	string line;
	vector<T>v;
	while(getline(cin, line)){
		long long x = stoll(line);
		if(x < numeric_limits<T>::min())
			throw runtime_error("The number \""+line+"\" is too small, min is \""+to_string(numeric_limits<T>::min())+"\"");
		if(x > numeric_limits<T>::max())
			throw runtime_error("The number \""+line+"\" is too large, max is \""+to_string(numeric_limits<T>::max())+"\"");
		v.push_back(x);
	}
	save_vector(output_vector_file, v);
}

template<class T>
void convert_float_data(const string&output_vector_file){
	string line;
	vector<T>v;
	while(getline(cin, line)){
		v.push_back(stold(line));
	}
	save_vector(output_vector_file, v);
}

void convert_uint64_data(const string&output_vector_file){
	string line;
	vector<unsigned long long>v;
	while(getline(cin, line)){
		v.push_back(stoull(line));
	}
	save_vector(output_vector_file, v);
}

string replace_all_substrings(string subject, const string& search, const string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return std::move(subject);
}

void convert_string_data(const string&output_vector_file){
	string line;
	vector<string>v;
	while(getline(cin, line)){
		v.push_back(replace_all_substrings(replace_all_substrings(line, "\\\\", "\\"), "\\n", "\n"));
	}
	save_vector(output_vector_file, v);
}

int main(int argc, char*argv[]){
	try{
		string data_type, output_vector_file;
		if(argc != 3){
			cerr
				<< "Usage: "<< argv[0] << " data_type output_vector_file\n"
				<< "\n"
				<< "Reads textual data from the standard input and writes it in a binary format to output_vector_file. The input data should be one data element per line. The data is only written once an end of file is encountered on the input. data_type can be one of\n"
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
			output_vector_file = argv[2];
		}

		if(data_type == "int8")
			convert_int_data<signed char>(output_vector_file);
		else if(data_type == "uint8")
			convert_int_data<unsigned char>(output_vector_file);
		else if(data_type == "int16")
			convert_int_data<signed short>(output_vector_file);
		else if(data_type == "uint16")
			convert_int_data<unsigned short>(output_vector_file);
		else if(data_type == "int32")
			convert_int_data<signed int>(output_vector_file);
		else if(data_type == "uint32")
			convert_int_data<unsigned int>(output_vector_file);
		else if(data_type == "int64")
			convert_int_data<signed long long>(output_vector_file);
		else if(data_type == "uint64")
			convert_uint64_data(output_vector_file);
		else if(data_type == "float32")
			convert_float_data<float>(output_vector_file);
		else if(data_type == "float64")
			convert_float_data<double>(output_vector_file);
		else if(data_type == "string")
			convert_string_data(output_vector_file);
		else
			throw runtime_error("Unknown data type \""+data_type+"\"");
	}catch(exception&err){
		cerr << "Stopped on exception : "<< err.what() << endl;
	}
}
