#include <routingkit/bit_vector.h>

#include "expect.h"
#include <iostream>

using namespace RoutingKit;
using namespace std;

int main(int argc, char*argv[]){

	{
		BitVector v;
	}

	{
		BitVector v(7777, false);
		EXPECT(v.size() == 7777);
		for(unsigned i=0; i<v.size(); ++i)
			EXPECT(!v.is_set(i));
	}

	{
		BitVector v(0, false);
	}

	{
		BitVector v(0, true);
	}

	{
		BitVector v(0, BitVector::uninitialized);
	}

	{
		BitVector v(0);
		v.resize(30, false);
	}

	{
		BitVector v(0);
		v.resize(300, true);
	}

	{
		BitVector v(0);
		v.resize(500, BitVector::uninitialized);
	}

	{
		BitVector v(100);
		v.resize(0, false);
	}

	{
		BitVector v(1000);
		v.resize(0, true);
	}


	{
		BitVector v(1000);
		v.set(501);
		v.set(500);
		v.set(498);
		v.set(2);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

		v.set(500);
		
		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

		v.reset(500);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(!v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));
	
		v.reset(500);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(!v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));
		
		v.set(500, true);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

		v.set(500, true);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

		v.set(500, false);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(!v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

		v.set(500, false);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(!v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

		v.toggle(500);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

		v.toggle(500);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(!v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

		v.toggle(500);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

		v.toggle(500);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(!v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));


		v.reset(500);
		v.set_if(500, false);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(!v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

		v.reset(500);
		v.set_if(500, true);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

		v.set(500);
		v.set_if(500, false);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

		v.set(500);
		v.set_if(500, true);

		EXPECT(!v.is_set(502));
		EXPECT(v.is_set(501));
		EXPECT(v.is_set(500));
		EXPECT(!v.is_set(499));
		EXPECT(v.is_set(498));
		EXPECT(v.is_set(2));
		EXPECT(!v.is_set(1));

	}

	{
		BitVector v(10000);
		v.resize(0, BitVector::uninitialized);
	}


	{
		BitVector v(7777, true);
		EXPECT(v.size() == 7777);
		for(unsigned i=0; i<v.size(); ++i)
			EXPECT(v.is_set(i));
	}


	{
		BitVector x(7777, true);
		BitVector y = x;

		EXPECT(x.size() == 7777);
		for(unsigned i=0; i<x.size(); ++i)
			EXPECT(x.is_set(i));
		EXPECT(y.size() == 7777);
		for(unsigned i=0; i<y.size(); ++i)
			EXPECT(y.is_set(i));
	}

	{
		BitVector x(7777, true);
		BitVector y = std::move(x);

		EXPECT(x.empty());
		EXPECT(y.size() == 7777);
		for(unsigned i=0; i<y.size(); ++i)
			EXPECT(y.is_set(i));
	}

	{
		BitVector x(7777, true);
		BitVector y(6666, false);

		EXPECT(x.size() == 7777);
		for(unsigned i=0; i<x.size(); ++i)
			EXPECT(x.is_set(i));
		EXPECT(y.size() == 6666);
		for(unsigned i=0; i<y.size(); ++i)
			EXPECT(!y.is_set(i));

		x.swap(y);

		EXPECT(y.size() == 7777);
		for(unsigned i=0; i<y.size(); ++i)
			EXPECT(y.is_set(i));
		EXPECT(x.size() == 6666);
		for(unsigned i=0; i<x.size(); ++i)
			EXPECT(!x.is_set(i));
	}

	{
		BitVector x(7777, true);
		
		EXPECT(x.is_set(0));
		EXPECT(x.is_any_set());
		EXPECT(x.are_all_set());
	}

	{
		BitVector x(7777, false);
		
		EXPECT(!x.is_any_set());
		EXPECT(!x.are_all_set());
	}

	{
		BitVector x(7777, false);
		
		x.set(42);

		EXPECT(x.is_any_set());
		EXPECT(!x.are_all_set());
	}

	{
		BitVector x(7777, false);
		
		EXPECT(!x.is_set(420));
		x.set(420);
		EXPECT(!x.is_set(419));
		EXPECT(x.is_set(420));
		EXPECT(!x.is_set(421));
		x.reset(420);
		EXPECT(!x.is_set(419));
		EXPECT(!x.is_set(420));
		EXPECT(!x.is_set(421));
		x.toggle(420);
		EXPECT(!x.is_set(419));
		EXPECT(x.is_set(420));
		EXPECT(!x.is_set(421));
	}

	{
		auto v = ~BitVector(7777, true);
		EXPECT(v.size() == 7777);
		for(unsigned i=0; i<v.size(); ++i)
			EXPECT(!v.is_set(i));

		v.reset_all();
		for(unsigned i=0; i<v.size(); ++i)
			EXPECT(!v.is_set(i));

		v.set_all();
		for(unsigned i=0; i<v.size(); ++i)
			EXPECT(v.is_set(i));

		v.reset_all();
		for(unsigned i=0; i<v.size(); ++i)
			EXPECT(!v.is_set(i));

	}

	{
		BitVector x(7777, true);
		BitVector y(6666, false);

		EXPECT(x!=y);
		
		y = BitVector(7777, true);

		EXPECT(x==y);

		y.inplace_not();

		EXPECT(x!=y);
	}

	{
		BitVector x(7777, true);
		BitVector y(6666, false);

		EXPECT(x!=y);
		
		y = BitVector(7777, true);

		EXPECT(x==y);

		y.inplace_not();

		EXPECT(x!=y);
	}

	{
		BitVector x(7777, true);
		BitVector y(6666, false);

		x.reset(42);

		EXPECT_CMP(x.population_count(), ==, 7776);
		EXPECT_CMP(y.population_count(), ==, 0);
	}

	{
		BitVector x(7777, true);
		
		EXPECT_CMP(x.size(), ==, 7777);
		EXPECT_CMP(x.population_count(), ==, 7777);
		EXPECT(x.are_all_set());
		
		x.resize(3000);

		EXPECT_CMP(x.size(), ==, 3000);
		EXPECT_CMP(x.population_count(), ==, 3000);
		EXPECT(x.are_all_set());
	
		x.resize(3001, true);

		EXPECT_CMP(x.size(), ==, 3001);
		EXPECT_CMP(x.population_count(), ==, 3001);
		EXPECT(x.are_all_set());
	
		x.resize(7777, true);

		EXPECT_CMP(x.size(), ==, 7777);
		EXPECT_CMP(x.population_count(), ==, 7777);
		EXPECT(x.are_all_set());
	
	}


	{
		BitVector v(12345);
		for(uint64_t i=0; i<12345; ++i)
			v.set(i, i%17 == 0);
		for(uint64_t i=0; i<12345; ++i){
			if(i%17 == 0)
				EXPECT(v.is_set(i));
			else
				EXPECT(!v.is_set(i));
		}
	}

	{
		auto x = make_bit_vector(12345, [](uint64_t x){return x%17 == 0;});
		for(uint64_t i=0; i<12345; ++i){
			if(i%17 == 0)
				EXPECT(x.is_set(i));
			else
				EXPECT(!x.is_set(i));
		}
	}

	{
		auto
			x = make_bit_vector(12345, [](uint64_t x){return x%2 == 0;}),
			y = make_bit_vector(12345, [](uint64_t x){return x%2 != 0;});

		EXPECT((x|y).are_all_set());
		EXPECT((x^y).are_all_set());
		EXPECT(!(x&y).is_any_set());
	}

	{
		BitVector v;

		v.make_large_enough_for(5);
		EXPECT_CMP(v.size(), >, 5);

		v.make_large_enough_for(8);
		EXPECT_CMP(v.size(), >, 8);

		v.make_large_enough_for(15);
		EXPECT_CMP(v.size(), >, 15);

		v.make_large_enough_for(1024);
		EXPECT_CMP(v.size(), >, 1024);
	}

	{
		BitVector v;

		v.make_large_enough_for(5, BitVector::uninitialized);
		EXPECT_CMP(v.size(), >, 5);

		v.make_large_enough_for(8, BitVector::uninitialized);
		EXPECT_CMP(v.size(), >, 8);

		v.make_large_enough_for(15, BitVector::uninitialized);
		EXPECT_CMP(v.size(), >, 15);

		v.make_large_enough_for(1024, BitVector::uninitialized);
		EXPECT_CMP(v.size(), >, 1024);
	}

	return expect_failed;
}
