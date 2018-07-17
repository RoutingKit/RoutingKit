#include "file_data_source.h"
#include <stdexcept>
#include <string>
#include <assert.h>

#ifndef ROUTING_KIT_NO_POSIX
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <string.h>

namespace RoutingKit{

#ifndef ROUTING_KIT_NO_POSIX

FileDataSource::FileDataSource():
	file_descriptor(-1){
}

FileDataSource::FileDataSource(const char*file_name):
	file_descriptor(-1){
	open(file_name);
}

FileDataSource::FileDataSource(const std::string&file_name):
	file_descriptor(-1){
	open(file_name);
}

FileDataSource::FileDataSource(FileDataSource&&o):
	file_descriptor(o.file_descriptor){
	o.file_descriptor = -1;
}

const FileDataSource&FileDataSource::operator=(FileDataSource&&o){
	file_descriptor = o.file_descriptor;
	o.file_descriptor = -1;
	return *this;
}


void FileDataSource::open(const char*file_name){
	int new_file_descriptor = ::open(file_name, O_RDONLY);
	if(new_file_descriptor == -1){
		int error = errno;
		throw std::runtime_error(std::string("Could not open file \"")+file_name +"\" for reading. The errno is "+std::to_string(error)+". strerror(errno) says the following : "+strerror(error));
	}
	if(file_descriptor != -1)
		::close(file_descriptor);
	file_descriptor = new_file_descriptor;
}

void FileDataSource::close(){
	if(file_descriptor != -1){
		::close(file_descriptor);
		file_descriptor = -1;
	}
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

#else

FileDataSource::FileDataSource():
	file_descriptor(nullptr){
}

FileDataSource::FileDataSource(const char*file_name):
	file_descriptor(nullptr){
	open(file_name);
}

FileDataSource::FileDataSource(const std::string&file_name):
	file_descriptor(nullptr){
	open(file_name);
}

FileDataSource::FileDataSource(FileDataSource&&o):
	file_descriptor(o.file_descriptor){
	o.file_descriptor = nullptr;
}

const FileDataSource&FileDataSource::operator=(FileDataSource&&o){
	file_descriptor = o.file_descriptor;
	o.file_descriptor = nullptr;
	return *this;
}

void FileDataSource::open(const char*file_name){
	FILE*new_file_descriptor = fopen(file_name, "rb");
	if(new_file_descriptor == nullptr)
		throw std::runtime_error(std::string("Could not open file \"")+file_name +"\" for reading.");
	if(file_descriptor != nullptr)
		fclose(file_descriptor);
	file_descriptor = new_file_descriptor;
}

void FileDataSource::close(){
	if(file_descriptor != nullptr){
		fclose(file_descriptor);
		file_descriptor = nullptr;
	}
}

void FileDataSource::rewind(){
	if(fseek(file_descriptor, 0, SEEK_SET))
		throw std::runtime_error("Could not rewind file opened for reading.");
}

unsigned long long FileDataSource::size()const{
	if(file_descriptor == nullptr)
		return 0;
	else{
		auto x = ftell(file_descriptor);
		if(x == -1L)
			throw std::runtime_error("ftell failed on file opened for reading.");
		if(fseek(file_descriptor, 0, SEEK_END))
			throw std::runtime_error("fseek failed on file opened for reading.");

		unsigned long long size = ftell(file_descriptor);
		if(fseek(file_descriptor, x, SEEK_SET))
			throw std::runtime_error("fseek failed on file opened for reading, file stream could be in an unexpected state.");

		if(x == -1L)
			throw std::runtime_error("ftell failed on file opened for reading, file stream could be in an unexpected state.");

		return size;
	}
}

namespace{
	unsigned long long my_read(FILE* file_descriptor, char*buffer, unsigned long long to_read){
		size_t s = ::fread(buffer, 1, to_read, file_descriptor);
		int err = ferror(file_descriptor);
		if(err != 0)
			throw std::runtime_error("fread failed on file opened for reading. ferror returns "+std::to_string(err)+".");
		return s;
	}
}

#endif

unsigned long long FileDataSource::read(char*buffer, unsigned long long to_read){
	return my_read(file_descriptor, buffer, to_read);
}

std::function<unsigned long long(char*, unsigned long long)>FileDataSource::get_read_function_object(){
	auto x = file_descriptor; // C++ or GCC 4.8 (I do not know who is at fault) does not like capturing members by value...
	return [x](char*buffer, unsigned long long to_read)->unsigned long long{
		return my_read(x, buffer, to_read);
	};
}

} // namespace RoutingKit

