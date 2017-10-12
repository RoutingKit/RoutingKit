#include <routingkit/bit_vector.h>
#include <routingkit/id_mapper.h>
#include <routingkit/timer.h>

#include "bit_select.h"
#include "expect.h"

#include <iostream>

using namespace RoutingKit;
using namespace std;

int main(){
	try{
		{
			cout << "Testing uint64_bit_select"<< endl;
			// 232226 == 00111000101100100010 binary
			EXPECT_CMP(uint64_bit_select(232226, 0), ==, 1);
			EXPECT_CMP(uint64_bit_select(232226, 1), ==, 5);
			EXPECT_CMP(uint64_bit_select(232226, 2), ==, 8);
			EXPECT_CMP(uint64_bit_select(232226, 3), ==, 9);
			EXPECT_CMP(uint64_bit_select(232226, 4), ==, 11);
			EXPECT_CMP(uint64_bit_select(232226, 5), ==, 15);
			EXPECT_CMP(uint64_bit_select(232226, 6), ==, 16);
			EXPECT_CMP(uint64_bit_select(232226, 7), ==, 17);

			EXPECT_CMP(uint64_bit_select(0xF, 0), ==, 0);
			EXPECT_CMP(uint64_bit_select(0xF, 1), ==, 1);
			EXPECT_CMP(uint64_bit_select(0xF, 2), ==, 2);
			EXPECT_CMP(uint64_bit_select(0xF, 3), ==, 3);
	
			EXPECT_CMP(uint64_bit_select(0xFF, 4), ==, 4);
			EXPECT_CMP(uint64_bit_select(0xFF, 5), ==, 5);
			EXPECT_CMP(uint64_bit_select(0xFF, 6), ==, 6);
			EXPECT_CMP(uint64_bit_select(0xFF, 7), ==, 7);

			EXPECT_CMP(uint64_bit_select(0xFF02, 0), ==, 1);
			EXPECT_CMP(uint64_bit_select(0xFF02, 1), ==, 8);
			EXPECT_CMP(uint64_bit_select(0xFF02, 2), ==, 9);
			EXPECT_CMP(uint64_bit_select(0xFF02, 3), ==, 10);
			EXPECT_CMP(uint64_bit_select(0xFF02, 4), ==, 11);
			EXPECT_CMP(uint64_bit_select(0xFF02, 5), ==, 12);
			EXPECT_CMP(uint64_bit_select(0xFF02, 6), ==, 13);
			EXPECT_CMP(uint64_bit_select(0xFF02, 7), ==, 14);
			EXPECT_CMP(uint64_bit_select(0xFF02, 8), ==, 15);

			EXPECT_CMP(uint64_bit_select(0xFFFF02, 15), ==, 22);
			EXPECT_CMP(uint64_bit_select(0xFFFF02, 16), ==, 23);
		}

		{
			cout << "Testing uint512_bit_select"<< endl;
			uint64_t block[] = {0xF, 232226, 232226, 0, 0, 0, 0, 0};
			EXPECT_CMP(uint512_bit_select(block, 0), ==, 0);
			EXPECT_CMP(uint512_bit_select(block, 1), ==, 1);
			EXPECT_CMP(uint512_bit_select(block, 2), ==, 2);
			EXPECT_CMP(uint512_bit_select(block, 3), ==, 3);

			EXPECT_CMP(uint512_bit_select(block, 4+0), ==, 64+1);
			EXPECT_CMP(uint512_bit_select(block, 4+1), ==, 64+5);
			EXPECT_CMP(uint512_bit_select(block, 4+2), ==, 64+8);
			EXPECT_CMP(uint512_bit_select(block, 4+3), ==, 64+9);
			EXPECT_CMP(uint512_bit_select(block, 4+4), ==, 64+11);
			EXPECT_CMP(uint512_bit_select(block, 4+5), ==, 64+15);
			EXPECT_CMP(uint512_bit_select(block, 4+6), ==, 64+16);
			EXPECT_CMP(uint512_bit_select(block, 4+7), ==, 64+17);

			EXPECT_CMP(uint512_bit_select(block, 12+0), ==, 128+1);
			EXPECT_CMP(uint512_bit_select(block, 12+1), ==, 128+5);
			EXPECT_CMP(uint512_bit_select(block, 12+2), ==, 128+8);
			EXPECT_CMP(uint512_bit_select(block, 12+3), ==, 128+9);
			EXPECT_CMP(uint512_bit_select(block, 12+4), ==, 128+11);
			EXPECT_CMP(uint512_bit_select(block, 12+5), ==, 128+15);
			EXPECT_CMP(uint512_bit_select(block, 12+6), ==, 128+16);
			EXPECT_CMP(uint512_bit_select(block, 12+7), ==, 128+17);
		}

		{
			cout << "Testing uint512_bit_select again"<< endl;
			uint64_t block[] = {0, 0, 0, 0, 0, 1, 0, 0};
			EXPECT_CMP(uint512_bit_select(block, 0), ==, 5*64);
		}


		{
			cout << "Testing bit_select"<< endl;

			uint64_t uint512_count = 14;

			uint64_t data[] = {
				0, 0, 0, 0, 0, 0, 0, 0,
				1, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 1, 0, 0, 0, 0, 0
			};

			uint64_t rank[] = {
				0,0,1,1,1,2,2,2,2,3,3,3,3,3,4
			};

			EXPECT_CMP(uint512_count*8, ==, sizeof(data)/8);
			EXPECT_CMP(uint512_count+1, ==, sizeof(rank)/8);
		
			EXPECT_CMP(bit_select(uint512_count, rank, data, 0), ==, 1*512+0*64);
			EXPECT_CMP(bit_select(uint512_count, rank, data, 1), ==, 4*512+2*64);
			EXPECT_CMP(bit_select(uint512_count, rank, data, 2), ==, 8*512+5*64);
			EXPECT_CMP(bit_select(uint512_count, rank, data, 3), ==, 13*512+2*64);
	
		}

		{
			cout << "Building test bit vectors"<< endl;
			
			BitVector single_bit_vector(1000000, false);
			single_bit_vector.set(555555);

			BitVector sparse_regular_vector = make_bit_vector(9999999, [](uint64_t x){ return x % 7919 == 0;});
			BitVector dense_regular_vector = make_bit_vector(8888888, [](uint64_t x){ return x % 7 == 0;});
		
			BitVector dense_irregular_vector = make_bit_vector(
				7777777,
				[](uint64_t x){
					if(((x/1000000) % 2) == 0)
						return false;
					else
						return x % 3 == 0;

				}
			);

			BitVector sparse_irregular_vector = make_bit_vector(
				18978978,
				[](uint64_t x){
					if(((x/1000000) % 2) != 0)
						return false;
					else
						return x % 6917 == 0;

				}
			);

			{
				cout << "Start testing is_global_id_mapped"<< endl;
				auto test_is_global_id_mapped = [&](const BitVector&vec){
					IDMapper map(vec);
					for(uint64_t x=0; x<map.global_id_count(); ++x){
						EXPECT_CMP(vec.is_set(x), ==, map.is_global_id_mapped(x));
					}
				};
				test_is_global_id_mapped(single_bit_vector);
				test_is_global_id_mapped(sparse_regular_vector);
				test_is_global_id_mapped(dense_regular_vector);
				test_is_global_id_mapped(sparse_irregular_vector);
				test_is_global_id_mapped(dense_irregular_vector);
			}

			{
				cout << "Start testing global_to_local"<< endl;
				auto test_global_to_local = [&](const BitVector&vec){
					IDMapper map(vec);
					EXPECT_CMP(map.local_id_count(), !=, 0);

					uint64_t s = 0;
					for(uint64_t x=0; x<map.global_id_count(); ++x){
						if(vec.is_set(x)){
							EXPECT_CMP(s, ==, map.to_local(x, ~0ull));
							++s;
						} else {
							EXPECT_CMP(~0ull, ==, map.to_local(x, ~0ull));
						}
					}
					EXPECT_CMP(map.local_id_count(), ==, s);

				};
				test_global_to_local(single_bit_vector);
				test_global_to_local(sparse_regular_vector);
				test_global_to_local(dense_regular_vector);
				test_global_to_local(sparse_irregular_vector);
				test_global_to_local(dense_irregular_vector);
			}

			{
				cout << "Start testing local_to_global"<< endl;
				auto test_local_to_global = [&](const BitVector&vec){
					IDMapper map(vec);
					EXPECT_CMP(map.local_id_count(), !=, 0);

					uint64_t s = 0;
					for(uint64_t x=0; x<map.global_id_count(); ++x){
						if(vec.is_set(x)){
							EXPECT_CMP(map.to_global(s), ==, x);
							++s;
						}
					}
					EXPECT_CMP(map.local_id_count(), ==, s);

				};
				test_local_to_global(single_bit_vector);
				test_local_to_global(sparse_regular_vector);
				test_local_to_global(dense_regular_vector);
				test_local_to_global(sparse_irregular_vector);
				test_local_to_global(dense_irregular_vector);
			}


			{
				cout << "Start testing bijection"<< endl;
				auto test_bijection = [&](const BitVector&vec){
					IDMapper map(vec);
					EXPECT(map.local_id_count() != 0);

					for(uint64_t x=0; x<map.local_id_count(); ++x){
						uint64_t y = map.to_global(x);
						EXPECT_CMP(y, !=, ~0ull);
						uint64_t z = map.to_local(y, ~0ull);
						EXPECT_CMP(z, !=, ~0ull);
						EXPECT_CMP(x, ==, z);
					}

					for(uint64_t x=0; x<map.global_id_count(); ++x){
						uint64_t y = map.to_local(x, ~0ull);
						if(y != ~0ull){
							uint64_t z = map.to_global(y);
							EXPECT_CMP(z, !=, ~0ull);
							EXPECT_CMP(x, ==, z);
						}
					}
				};
				test_bijection(single_bit_vector);
				test_bijection(sparse_regular_vector);
				test_bijection(dense_regular_vector);
				test_bijection(sparse_irregular_vector);
				test_bijection(dense_irregular_vector);
			}
		}


		{
			cout << "Start sparse speed test " << endl;
			BitVector bits = make_bit_vector(
				1856497897ull,
				[](uint64_t x){
					if(((x/10000000) % 2) != 0)
						return false;
					else
						return x % 6917 == 0;

				}
			);

			IDMapper map(bits);

			uint64_t junk = 0;

			{
				long long timer = -get_micro_time();
				unsigned n = 3000000;
				for(unsigned i=0; i<n; ++i)
					junk += map.to_local(std::rand()%map.global_id_count(), ~0ull);
				timer += get_micro_time();
				cout << "to_local running time : "<< (timer*1000/n) <<"ns"  << endl;
			}

			{
				long long timer = -get_micro_time();
				unsigned n = 3000000;
				for(unsigned i=0; i<n; ++i)
					junk += map.to_global(std::rand()%map.local_id_count());
				timer += get_micro_time();
				cout << "to_global running time : "<< (timer*1000/n) <<"ns"  << endl;
			}
			cout << "memory overhead : "<<static_cast<float>(map.memory_overhead_in_bits()) / static_cast<float>(map.global_id_count()) << "bits" << endl;

			cout << "output junk to stop optimizer " << junk << endl;
		
		}

		{
			cout << "Start dense speed test " << endl;
			BitVector bits = make_bit_vector(
				1856497897ull,
				[](uint64_t x){
					if(((x/10000000) % 2) != 0)
						return false;
					else
						return x % 7 != 0;

				}
			);

			IDMapper map(bits);

			uint64_t junk = 0;

			{
				long long timer = -get_micro_time();
				unsigned n = 500000;
				for(unsigned i=0; i<n; ++i)
					junk += map.to_local(std::rand()%map.global_id_count(), ~0ull);
				timer += get_micro_time();
				cout << "to_local running time : "<< (timer*1000/n) <<"ns"  << endl;
			}

			{
				long long timer = -get_micro_time();
				unsigned n = 500000;
				for(unsigned i=0; i<n; ++i)
					junk += map.to_global(std::rand()%map.local_id_count());
				timer += get_micro_time();
				cout << "to_global running time : "<< (timer*1000/n) <<"ns"  << endl;
			}
			cout << "memory overhead : "<<static_cast<float>(map.memory_overhead_in_bits()) / static_cast<float>(map.global_id_count()) << "bits" << endl;

			cout << "output junk to stop optimizer " << junk << endl;
		
		}


	}catch(std::exception&err){
		cout << "exception" << ":" << err.what() << endl;
		return 1;
	}
	return expect_failed;
}

