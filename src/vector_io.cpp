#include <routingkit/vector_io.h>

namespace RoutingKit{

template<>
void save_vector<std::string>(const std::string&file_name, const std::vector<std::string>&vec){
	std::ofstream out(file_name, std::ios::binary);
	for(unsigned i=0; i<vec.size(); ++i){
		const char*x = vec[i].c_str();
		out.write(x, vec[i].length()+1);
	}
}

template<>
std::vector<std::string>load_vector<std::string>(const std::string&file_name){
	std::vector<char>data = load_vector<char>(file_name);
	std::vector<std::string>ret;
	std::vector<char>::const_iterator
		str_begin = data.begin(),
		str_end = data.begin(),
		data_end = data.end();

	while(str_end != data_end){
		if(*str_end == '\0'){
			ret.push_back(std::string(str_begin, str_end));
			++str_end;
			str_begin = str_end;
		}else{
			++str_end;
		}
	}

	ret.shrink_to_fit();
	return ret; // NVRO
}

void save_bit_vector(const std::string&file_name, const BitVector&vec){
	std::ofstream out(file_name, std::ios::binary);
	if(!out)
		throw std::runtime_error("Can not open \""+file_name+"\" for writing.");
	uint64_t size = vec.size();
	out.write(reinterpret_cast<const char*>(&size), 8);
	out.write(reinterpret_cast<const char*>(vec.data()), vec.uint512_count()*64);
}

BitVector load_bit_vector(const std::string&file_name){
	std::ifstream in(file_name, std::ios::binary);
	if(!in)
		throw std::runtime_error("Can not open \""+file_name+"\" for reading.");
	in.seekg(0, std::ios::end);
	unsigned long long file_size = in.tellg();
	in.seekg(0, std::ios::beg);
	
	uint64_t size;
	in.read(reinterpret_cast<char*>(&size), 8);
	if(((size+511)/512) * 64 + 8 != file_size)
		throw std::runtime_error("File \""+file_name+"\" can not be a bit vector of the requested size because the size in the header and the file size do not correspond.");
	BitVector vec(size);
	in.read(reinterpret_cast<char*>(vec.data()), ((size+511)/512));
	return vec; // NVRO
}

} // RoutingKit

