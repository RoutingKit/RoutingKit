#include "file_data_source.h"
#include <stdexcept>
#include <string>
#include <assert.h>


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

namespace RoutingKit{

void FileDataSource::open(const char*file_name){
	file_descriptor = ::open(file_name, O_RDONLY);
	if(file_descriptor == -1){
		int error = errno;
		throw std::runtime_error(std::string("Could not open file \"")+file_name +"\" for reading. The errno is "+std::to_string(error)+". strerror(errno) says the following : "+strerror(error));
	}
}

void FileDataSource::close(){
	::close(file_descriptor);
}

void FileDataSource::rewind(){
	if(::lseek(file_descriptor, 0, SEEK_SET) == -1){
		int error = errno;
		throw std::runtime_error(std::string("Could not rewind a file open for reading. The errno is ")+std::to_string(error)+". strerror(errno) says the following : "+strerror(error));
	}
}

unsigned long long FileDataSource::size()const{
	if(file_descriptor == -1)
		return 0;
	else{
		struct ::stat buf;
		if(!::fstat(file_descriptor, &buf)){
			int error = errno;
			throw std::runtime_error(std::string("Could not determine the size of a file open for reading. The errno is ")+std::to_string(error)+". strerror(errno) says the following : "+strerror(error));
		}
		return buf.st_size;
	}
}

namespace{
	unsigned long long my_read(int file_descriptor, char*buffer, unsigned long long to_read){
		ssize_t s = ::read(file_descriptor, buffer, to_read);
		if(s < 0){
			int error = errno;
			throw std::runtime_error(std::string("Could not read from file. The errno is ")+std::to_string(error)+". strerror(errno) says the following : "+strerror(error));
		}
		return s;
	}
}

unsigned long long FileDataSource::read(char*buffer, unsigned long long to_read){
	return my_read(file_descriptor, buffer, to_read);
}

std::function<unsigned long long(char*, unsigned long long)>FileDataSource::get_read_function_object(){
	int x = file_descriptor; // C++ or GCC 4.8 (I do not know who is at fault) does not like capturing members by value...
	return [x](char*buffer, unsigned long long to_read)->unsigned long long{
		return my_read(x, buffer, to_read);
	};
}

} // namespace RoutingKit

