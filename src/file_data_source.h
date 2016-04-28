#ifndef FILE_DATA_SOURCE_H
#define FILE_DATA_SOURCE_H

#include <functional>
#include <string>

#ifdef ROUTING_KIT_NO_POSIX
#include <stdio.h>
#endif

namespace RoutingKit{

class FileDataSource{
public:
	FileDataSource();
	FileDataSource(const char*file_name);
	FileDataSource(const std::string&file_name);
	
	unsigned long long size()const;

	void rewind();

	void open(const char*file_name);
	void open(const std::string&file_name) {open(file_name.c_str());}

	void close();

	FileDataSource(const FileDataSource&) = delete;
	const FileDataSource&operator=(const FileDataSource&) = delete;

	FileDataSource(FileDataSource&&o);
	const FileDataSource&operator=(FileDataSource&&o);

	unsigned long long minimum_read_size() const {
		return 1;
	}

	unsigned long long read(char*buffer, unsigned long long to_read);

	std::function<unsigned long long(char*, unsigned long long)>get_read_function_object();

	~FileDataSource(){ close(); }
private:
	#ifndef ROUTING_KIT_NO_POSIX
	int file_descriptor;
	#else
	FILE*file_descriptor;
	#endif
};

} // RoutingKit

#endif
