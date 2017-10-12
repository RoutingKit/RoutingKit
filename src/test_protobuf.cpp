#include "protobuf.h"
#include "expect.h"
#include <limits>
#include <string.h>

#include <iostream>

#include <bitset>

using namespace RoutingKit;
using namespace std;

int main(){
	EXPECT_CMP(0, ==, zigzag_convert_uint64_to_int64(0ull));
	EXPECT_CMP(-1ll, ==, zigzag_convert_uint64_to_int64(1ull));
	EXPECT_CMP(1ll, ==, zigzag_convert_uint64_to_int64(2ull));
	EXPECT_CMP(-2ll, ==, zigzag_convert_uint64_to_int64(3ull));
	EXPECT_CMP(2147483647ll, ==, zigzag_convert_uint64_to_int64(4294967294ull));
	EXPECT_CMP(-2147483648ll, ==, zigzag_convert_uint64_to_int64(4294967295ull));
	EXPECT_CMP(numeric_limits<int64_t>::max(), ==, zigzag_convert_uint64_to_int64(numeric_limits<uint64_t>::max()-1));
	EXPECT_CMP(numeric_limits<int64_t>::min(), ==, zigzag_convert_uint64_to_int64(numeric_limits<uint64_t>::max()));


	{
		char buf[] =  {(char)172, (char)2};
		const char*begin = buf, *end = buf+sizeof(buf);
		auto val = decode_varint_as_uint64_and_advance_first_parameter(begin, end);
		EXPECT_CMP(val, ==, 300);
		EXPECT_CMP((void*)begin, ==, (void*)end);
	}

	{
		char buf[] =  {(char)100};
		const char*begin = buf, *end = buf+sizeof(buf);
		auto val = decode_varint_as_uint64_and_advance_first_parameter(begin, end);
		EXPECT_CMP(val, ==, 100);
		EXPECT_CMP((void*)begin, ==, (void*)end);
	}

	{
		char buf[] =  {(char)128, (char)128, (char)5};
		const char*begin = buf, *end = buf+sizeof(buf);
		auto val = decode_varint_as_uint64_and_advance_first_parameter(begin, end);
		EXPECT_CMP(val, ==, 81920);
		EXPECT_CMP((void*)begin, ==, (void*)end);
	}

	{
		char buf[] =  {(char)201, (char)205, (char)6};
		const char*begin = buf, *end = buf+sizeof(buf);
		auto val = decode_varint_as_uint64_and_advance_first_parameter(begin, end);
		EXPECT_CMP(val, ==, 108233);
		EXPECT_CMP((void*)begin, ==, (void*)end);
	}

	{
		char buf[] =  {(char)180, (char)179, (char)169, (char)134, (char)10};
		const char*begin = buf, *end = buf+sizeof(buf);
		auto val = decode_varint_as_uint64_and_advance_first_parameter(begin, end);
		EXPECT_CMP(val, ==, 1348807898ull*2);
		EXPECT_CMP((void*)begin, ==, (void*)end);
	}

	{
		char buf[] = {
			(char)138, (char)168, (char)17,
			(char)220, (char)254, (char)219, (char)195, (char)2,
			(char)199, (char)254, (char)219, (char)195, (char)2,
			(char)180, (char)179, (char)169, (char)134, (char)10,
			(char)141, (char)229, (char)205, (char)194, (char)7,
			(char)167, (char)206, (char)219, (char)195, (char)2
		};
		const char*begin = buf, *end = buf+sizeof(buf);
		
		int64_t	val[] = {141829, 339443630, -339443620, 1348807898, -1009367367, -339440532};
		unsigned val_size = sizeof(val)/sizeof(val[0]);
		unsigned i=0;		

		const char*val_begin[] = {buf+0, buf+3, buf+8, buf+13, buf+18, buf+23};

		while(begin != end){
			EXPECT_CMP(begin, ==, val_begin[i]);
			auto x = decode_varint_as_uint64_and_advance_first_parameter(begin, end);
			EXPECT_CMP(begin, <=, end);
			auto y = zigzag_convert_uint64_to_int64(x);
			EXPECT_CMP(y, ==, val[i]);
			EXPECT_CMP(i, <, val_size);
			++i;
		}
		EXPECT_CMP(i, ==, val_size);
	}

	{
		char buf[] = {0x12, 0x07, 0x74, 0x65, 0x73, 0x74, 0x69, 0x6e, 0x67};
		const char*begin = buf, *end = buf + sizeof(buf);

		bool first = true;
		decode_protobuf_message_with_callbacks(
			begin, end,
			[](uint64_t id, uint64_t num){
				EXPECT(false);
			},
			[](uint64_t id, double num){
				EXPECT(false);
			},
			[&](uint64_t id, const char*begin, const char*end){
				EXPECT_CMP(id, ==, 2);
				EXPECT(!memcmp(begin, "testing", end-begin));
				EXPECT(first);
				first = false;
			}
		);
		first = false;
	}

	return expect_failed;
}
