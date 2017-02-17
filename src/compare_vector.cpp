#include <routingkit/vector_io.h>

#include <vector>
#include <iostream>
#include <stdexcept>

using namespace RoutingKit;
using namespace std;

template<class T>
void compare_num_data(const string&vector1_file, const string&vector2_file){
	vector<T>vector1 = load_vector<T>(vector1_file);
	vector<T>vector2 = load_vector<T>(vector2_file);

	if(vector1.size() < vector2.size()){
		cout << "\""<< vector1_file<< "\" has only " << vector1.size() << " elements while \""<< vector2_file<< "\" has "<<vector2.size() << ". Can only compare vectors of equal size." << endl;
	}else if(vector2.size() < vector1.size()){
		cout << "\""<< vector2_file<< "\" has only " << vector2.size() << " elements while \""<< vector1_file<< "\" has "<<vector1.size() << ". Can only compare vectors of equal size." << endl;
	}else{
		unsigned vector1_smaller_count = 0;
		unsigned vector2_smaller_count = 0;

		unsigned first_difference = (unsigned)-1;

		for(unsigned i=0; i<vector1.size(); ++i){
			if(vector1[i] < vector2[i])
				++vector1_smaller_count;
			if(vector2[i] < vector1[i])
				++vector2_smaller_count;
			if(vector1[i] != vector2[i] && first_difference == (unsigned)-1)
				first_difference = i;
		}

		if(vector1_smaller_count == 0 && vector2_smaller_count == 0){
			cout << "The vectors are the same and have "<<vector1.size()<<" elements." << endl;
		}else{
			cout
				<< "The vectors differ. "
				<< vector1_smaller_count <<" elements are smaller in \""<<vector1_file<<"\". "
				<< vector2_smaller_count <<" elements are smaller in \""<<vector2_file<<"\". "
				<< (vector1.size()-vector1_smaller_count-vector2_smaller_count) <<" elements are the same. "
				<< (vector1_smaller_count+vector2_smaller_count) <<" elements are different. "
				<< "The vectors have "<<vector1.size() <<" elements. "
				<< "The first element that differs is at index "<< first_difference << "."
				<< endl
			;
		}
	}

}

void compare_string_data(const std::string&vector1_file, const std::string&vector2_file){
	vector<string>vector1 = load_vector<string>(vector1_file);
	vector<string>vector2 = load_vector<string>(vector2_file);

	if(vector1.size() < vector2.size()){
		cout << "\""<< vector1_file<< "\" has only " << vector1.size() << " elements while \""<< vector2_file<< "\" has "<<vector2.size() << ". Can only compare vectors of equal size." << endl;
	}else if(vector2.size() < vector1.size()){
		cout << "\""<< vector2_file<< "\" has only " << vector2.size() << " elements while \""<< vector1_file<< "\" has "<<vector1.size() << ". Can only compare vectors of equal size." << endl;
	}else{
		unsigned equal_count = 0;

		for(unsigned i=0; i<vector1.size(); ++i){
			if(vector1[i] == vector2[i])
				++equal_count;
		}

		if(equal_count == vector1.size()){
			cout << "The vectors are the same and have "<<vector1.size()<<" elements." << endl;
		}else{
			cout
				<< "The vectors differ. "
				<< equal_count <<" elements are the same. "
				<< vector1.size()-equal_count <<" elements are different. "
				<< "The vectors have "<<vector1.size() <<" elements."
				<< endl
			;
		}
	}

}

int main(int argc, char*argv[]){
	try{
		string vector1_file;
		string vector2_file;
		string data_type;
		if(argc != 4){
			cerr << "Usage: "<< argv[0] << " type vector1 vector2\n"
				<< "\n"
				<< "Compares two vectors of elements in binary format. data_type can be one of\n"
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
			vector1_file = argv[2];
			vector2_file = argv[3];
		}

		if(data_type == "int8")
			compare_num_data<signed char>(vector1_file, vector2_file);
		else if(data_type == "uint8")
			compare_num_data<unsigned char>(vector1_file, vector2_file);
		else if(data_type == "int16")
			compare_num_data<signed short>(vector1_file, vector2_file);
		else if(data_type == "uint16")
			compare_num_data<unsigned short>(vector1_file, vector2_file);
		else if(data_type == "int32")
			compare_num_data<signed int>(vector1_file, vector2_file);
		else if(data_type == "uint32")
			compare_num_data<unsigned int>(vector1_file, vector2_file);
		else if(data_type == "int64")
			compare_num_data<signed long long>(vector1_file, vector2_file);
		else if(data_type == "uint64")
			compare_num_data<unsigned long long>(vector1_file, vector2_file);
		else if(data_type == "float32")
			compare_num_data<float>(vector1_file, vector2_file);
		else if(data_type == "float64")
			compare_num_data<double>(vector1_file, vector2_file);
		else if(data_type == "string")
			compare_string_data(vector1_file, vector2_file);
		else
			throw runtime_error("Unknown data type \""+data_type+"\"");
	}catch(exception&err){
		cerr << "Stopped on exception : " << err.what() << endl;
	}
}
