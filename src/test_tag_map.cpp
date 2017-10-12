#include <routingkit/tag_map.h>

#include "expect.h"

using namespace RoutingKit;

int main(){
	
	TagMap map;

	EXPECT_CMP(map["test"], ==, nullptr);

	typedef const char* char_ptr; // typedef because I have no idea how the syntax for an array of pointers works without

	{
		char_ptr key [] = {"foo", "bar", "foobar"};
		char_ptr value [] = {"a", "b", "c"};

		map.build(3, [&](unsigned i){return key[i];}, [&](unsigned i){return value[i];});

		EXPECT_CMP(map["foo"], ==, value[0]);
		EXPECT_CMP(map["foo"], ==, value[0]);

		EXPECT_CMP(map["bar"], ==, value[1]);
		EXPECT_CMP(map["bar"], ==, value[1]);

		EXPECT_CMP(map["bar"], !=, value[0]);

		EXPECT_CMP(map["foobar"], ==, value[2]);

		EXPECT_CMP(map["barfoo"], ==, nullptr);
	}

	{
		char_ptr key [] = {"foo", "axbyc", "aybxc", "bar", "azbzc"};
		char_ptr value [] = {"a", "b", "c", "d", "e"};

		map.build(5, [&](unsigned i){return key[i];}, [&](unsigned i){return value[i];});

		EXPECT_CMP(map["foo"], ==, value[0]);
		EXPECT_CMP(map["axbyc"], ==, value[1]);
		EXPECT_CMP(map["aybxc"], ==, value[2]);
		EXPECT_CMP(map["bar"], ==, value[3]);
		EXPECT_CMP(map["azbzc"], ==, value[4]);

	}

	return expect_failed;
}
