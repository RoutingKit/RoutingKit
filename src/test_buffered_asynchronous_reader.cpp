#include "buffered_asynchronous_reader.h"

#include <iostream>
#include <stdexcept>

using namespace RoutingKit;
using namespace std;

int main(){
	try{

		{
			cout << "start exception test" << endl;

			struct test_exception{};


			unsigned long long file_length = (1<<30)-103;
			unsigned long long file_pos = 0;
			char file_char = 0;
			function<unsigned long long(char*, unsigned long long)>data_source = [&](char*buffer, unsigned long long amount_to_read){

				if(file_char > 0 && amount_to_read > (unsigned)file_char)
					amount_to_read -= file_char;

				unsigned long long amount_read = 0;
				while(amount_read < amount_to_read && file_pos < file_length){
					*buffer = file_char;
					++buffer;
					++file_char;
					++amount_read;
					++file_pos;
				}
				if(file_pos > 1<<20)
					throw test_exception();
				return amount_read;
			};

			try{
				unsigned long long block_size = (1<<20) - 43;
				BufferedAsynchronousReader reader(data_source, block_size);
				unsigned long long total_bytes_read = 0;
				for(;;){
					char*data = reader.read(min(block_size, file_length-total_bytes_read));
					if(data == nullptr)
						break;
					total_bytes_read += block_size;
				}

			}catch(test_exception){
				cout << "test_exception caught successfully" << endl;
			}

			cout << "exception test done" << endl;
		}

		{
			cout << "start stress test" << endl;

			unsigned long long file_length = (1<<30)-103;
			unsigned long long file_pos = 0;
			char file_char = 0;

			function<unsigned long long(char*, unsigned long long)>data_source = [&](char*buffer, unsigned long long amount_to_read){

				if(file_char > 0 && amount_to_read > (unsigned)file_char)
					amount_to_read -= file_char;

				unsigned long long amount_read = 0;
				while(amount_read < amount_to_read && file_pos < file_length){
					*buffer = file_char;
					++buffer;
					++file_char;
					++amount_read;
					++file_pos;
				}
				return amount_read;
			};

			unsigned long long block_size = (1<<20) - 43;
			BufferedAsynchronousReader reader(data_source, block_size);

			char read_char = 0;
			unsigned long long total_bytes_read = 0;

			for(;;){
				unsigned to_read = min(block_size, file_length-total_bytes_read);
				if(to_read == 0)
					break;
				char*ret = reader.read(to_read);
				if(ret == nullptr)
					break;

				for(unsigned i=0; i<to_read; ++i){
					if(*ret != read_char)
						throw runtime_error("read returned a wrong byte in the buffer");
					++ret;
					++read_char;
					++total_bytes_read;
				}
			}

			if(total_bytes_read != file_length)
				throw runtime_error("file_length is "+to_string(file_length) +" but "+to_string(total_bytes_read)+" bytes were read");

			if(reader.read(1) != 0)
				throw runtime_error("after the reading is finished read should return a nullptr.");
			cout << "stress test done" << endl;
		}
	}catch(exception&err){
		cout << "Error : "<< err.what() << endl;
		return 1;
	}
	return 0;
}

