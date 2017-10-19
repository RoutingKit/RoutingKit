#include <routingkit/sort.h>
#include <routingkit/permutation.h>
#include <routingkit/timer.h>

#include "expect.h"

#include <vector>
#include <stdlib.h>
#include <iostream>
#include <random>

using namespace RoutingKit;
using namespace std;

struct Point{
	double x, y;
};

bool operator==(Point l, Point r){
	return l.x == r.x && l.y == r.y;
}

int main(){

	{
		std::default_random_engine rng(42);

		unsigned range_list[] = {100, 1000000000};

		for(unsigned range:range_list){
			std::uniform_int_distribution<int> dist(0, range-1);

			vector<unsigned>v(10000);
			for(auto&x:v)
				x = dist(rng);

			auto q = compute_stable_sort_permutation_using_key(v, range, [](unsigned x){return x;});
			EXPECT(is_sorted_using_less(apply_permutation(q, v)));
			EXPECT(q == compute_stable_sort_permutation_using_less(v));
			EXPECT(q == compute_stable_sort_permutation_using_comparator(v, [](unsigned l, unsigned r){return l<r;}));
		}
	}

	{
		std::default_random_engine rng(42);

		unsigned range_list[] = {100, 1000000000};

		for(unsigned range:range_list){
			std::uniform_int_distribution<int> dist(0, range-1);

			vector<unsigned>v(10000);
			for(auto&x:v)
				x = dist(rng);

			auto q = compute_inverse_stable_sort_permutation_using_key(v, range, [](unsigned x){return x;});
			EXPECT(is_sorted_using_less(apply_inverse_permutation(q, v)));
			EXPECT(q == compute_inverse_stable_sort_permutation_using_less(v));
			EXPECT(q == compute_inverse_stable_sort_permutation_using_comparator(v, [](unsigned l, unsigned r){return l<r;}));
		}
	}


	{
		std::default_random_engine rng(42);

		unsigned range_list[] = {100, 1000000000};

		for(unsigned range:range_list){
			std::uniform_int_distribution<int> dist(0, range-1);

			vector<unsigned>v(10000);
			for(auto&x:v)
				x = dist(rng);

			EXPECT(is_sorted_using_less(apply_permutation(compute_sort_permutation_using_key(v, range, [](unsigned x){return x;}), v)));
			EXPECT(is_sorted_using_less(apply_permutation(compute_sort_permutation_using_less(v), v)));
			EXPECT(is_sorted_using_less(apply_permutation(compute_sort_permutation_using_comparator(v, [](unsigned l, unsigned r){return l<r;}), v)));
		}
	}

	{
		std::default_random_engine rng(42);

		unsigned range_list[] = {100, 1000000000};

		for(unsigned range:range_list){
			std::uniform_int_distribution<int> dist(0, range-1);

			vector<unsigned>v(10000);
			for(auto&x:v)
				x = dist(rng);

			EXPECT(is_sorted_using_less(apply_inverse_permutation(compute_inverse_sort_permutation_using_key(v, range, [](unsigned x){return x;}), v)));
			EXPECT(is_sorted_using_less(apply_inverse_permutation(compute_inverse_sort_permutation_using_less(v), v)));
			EXPECT(is_sorted_using_less(apply_inverse_permutation(compute_inverse_sort_permutation_using_comparator(v, [](unsigned l, unsigned r){return l<r;}), v)));
		}
	}






	{
		std::default_random_engine rng(42);
		std::uniform_int_distribution<int> uniform_dist(0, 99);

		vector<double>v(10000);
		for(auto&x:v)
			x = uniform_dist(rng);

		auto get_key = [](double x)->unsigned{return (unsigned)x;};
		unsigned key_count = 100;
		auto less_comparator = [](double x, double y){return x < y;};
		auto greater_comparator = [](double x, double y){return x > y;};


		{
			auto p = compute_sort_permutation_using_key(v, key_count, get_key);
			for(unsigned i=1; i<v.size(); ++i)
				EXPECT(v[p[i-1]] <= v[p[i]]);
		}

		vector<double>sorted_v = v;
		std::sort(sorted_v.begin(), sorted_v.end());


		EXPECT(
			sorted_v ==
			apply_permutation(
				compute_sort_permutation_using_key(v, key_count, get_key),
				v
			)
		);

		EXPECT(
			sorted_v ==
			apply_inverse_permutation(
				compute_inverse_sort_permutation_using_key(v, key_count, get_key),
				v
			)
		);

		EXPECT(
			sorted_v ==
			apply_permutation(
				compute_stable_sort_permutation_using_key(v, key_count, get_key),
				v
			)
		);

		EXPECT(
			sorted_v ==
			apply_inverse_permutation(
				compute_inverse_stable_sort_permutation_using_key(v, key_count, get_key),
				v
			)
		);

		EXPECT(
			is_sorted_using_key(sorted_v, key_count, get_key)
		);


		EXPECT(
			sorted_v ==
			apply_permutation(
				compute_sort_permutation_using_comparator(v, less_comparator),
				v
			)
		);

		EXPECT(
			sorted_v ==
			apply_permutation(
				compute_stable_sort_permutation_using_comparator(v, less_comparator),
				v
			)
		);

		EXPECT(
			sorted_v ==
			apply_permutation(
				compute_sort_permutation_using_less(v),
				v
			)
		);

		EXPECT(
			sorted_v ==
			apply_permutation(
				compute_stable_sort_permutation_using_less(v),
				v
			)
		);

		EXPECT(
			is_sorted_using_less(sorted_v)
		);

		EXPECT(
			is_sorted_using_comparator(sorted_v, less_comparator)
		);

		std::reverse(sorted_v.begin(), sorted_v.end());

		EXPECT(
			sorted_v ==
			apply_permutation(
				compute_sort_permutation_using_comparator(v, greater_comparator),
				v
			)
		);

		EXPECT(
			sorted_v ==
			apply_permutation(
				compute_stable_sort_permutation_using_comparator(v, greater_comparator),
				v
			)
		);

		EXPECT(
			is_sorted_using_comparator(sorted_v, greater_comparator)
		);

	}

	{
		std::default_random_engine rng(42);

		std::vector<unsigned>v = random_permutation(100, rng);

		auto p1 = compute_stable_sort_permutation_using_key(v, 100, [&](unsigned x){return x;});
		auto p2 = compute_stable_sort_permutation_using_key(identity_permutation(100), 100, [&](unsigned x){return v[x];});

		auto r = random_permutation(100, rng);
		auto s = compute_stable_sort_permutation_using_key(r, 100, [&](unsigned x){return v[x];});
		auto p3 = chain_permutation_first_left_then_right(r, s);

		EXPECT(is_sorted_using_less(apply_permutation(p1, v)));
		EXPECT(is_sorted_using_less(apply_permutation(p2, v)));
		EXPECT(is_sorted_using_less(apply_permutation(p3, v)));

		EXPECT(p1 == p2);
		EXPECT(p1 == p3);
	}

	{
		std::default_random_engine rng(42);
		unsigned coord_count = 100;
		std::uniform_int_distribution<int> uniform_dist(0, coord_count-1);

		vector<Point>v(10000);

		for(auto&p:v){
			p.x = uniform_dist(rng);
			p.y = uniform_dist(rng);
		}
		auto get_x_key = [](Point p)->unsigned{return (unsigned)p.x;};
		unsigned key_count = coord_count;
		auto x_less_comparator = [](Point l, Point r){return l.x < r.x;};


		vector<Point>sorted_v = v;
		std::stable_sort(sorted_v.begin(), sorted_v.end(), x_less_comparator);

		EXPECT(
			sorted_v ==
			apply_permutation(
				compute_stable_sort_permutation_using_key(v, key_count, get_x_key),
				v
			)
		);

		EXPECT(
			sorted_v ==
			apply_inverse_permutation(
				compute_inverse_stable_sort_permutation_using_key(v, key_count, get_x_key),
				v
			)
		);

		EXPECT(
			sorted_v ==
			apply_permutation(
				compute_stable_sort_permutation_using_comparator(v, x_less_comparator),
				v
			)
		);
	}

	{
		auto print_something_to_prevent_optimizer_from_removing_everything = [&](const std::vector<unsigned>&v){
			unsigned x = 0;
			for(auto&y:v)
				x ^= y;
			cout << "Safty junk: " << x << endl;
		};

		std::default_random_engine rng(42);
		unsigned element_count = 100000000;
		unsigned key_count = 100;
		std::uniform_int_distribution<int> uniform_dist(0, key_count-1);

		vector<unsigned>elements(element_count);
		for(unsigned i=0; i<element_count; ++i)
			elements[i] = uniform_dist(rng);

		long long time = -get_micro_time();
		auto p = compute_sort_permutation_using_key(elements, key_count, [](unsigned x){return x;});
		time += get_micro_time();
		print_something_to_prevent_optimizer_from_removing_everything(p);
		cout << "compute_sort_permutation_using_key " << time << "musec" << endl;

		time = -get_micro_time();
		auto inv_p = compute_inverse_sort_permutation_using_key(elements, key_count, [](unsigned x){return x;});
		time += get_micro_time();
		print_something_to_prevent_optimizer_from_removing_everything(inv_p);
		cout << "compute_inverse_sort_permutation_using_key " << time << "musec" << endl;

		time = -get_micro_time();
		auto inv_p2 = invert_permutation(p);
		time += get_micro_time();
		EXPECT(inv_p2 == inv_p);
		print_something_to_prevent_optimizer_from_removing_everything(inv_p2);
		cout << "invert_permutation " << time << "musec" << endl;

		time = -get_micro_time();
		auto foo = apply_permutation(p, elements);
		time += get_micro_time();
		print_something_to_prevent_optimizer_from_removing_everything(foo);
		cout << "apply_permutation " << time << "musec" << endl;

		time = -get_micro_time();
		foo = apply_inverse_permutation(inv_p, elements);
		time += get_micro_time();
		print_something_to_prevent_optimizer_from_removing_everything(foo);
		cout << "apply_inverse_permutation " << time << "musec" << endl;

		/*vector<unsigned>elements2 = elements;

		time = -get_micro_time();
		inplace_apply_inverse_permutation(inv_p, elements2);
		time += get_micro_time();
		cout << "inplace_apply_inverse_permutation " << time << "musec" << endl;
	*/
		time = -get_micro_time();
		foo = sort_using_key(elements, key_count, [](unsigned x){return x;});
		time += get_micro_time();
		print_something_to_prevent_optimizer_from_removing_everything(foo);
		cout << "sort_using_key " << time << "musec" << endl;

		time = -get_micro_time();
		foo = apply_inverse_permutation(compute_inverse_sort_permutation_using_key(elements, key_count, [](unsigned x){return x;}), elements);
		time += get_micro_time();
		print_something_to_prevent_optimizer_from_removing_everything(foo);
		cout << "apply_inverse_permutation + compute_sort_permutation_using_key " << time << "musec" << endl;


		time = -get_micro_time();
		foo = sort_using_less(elements);
		time += get_micro_time();
		print_something_to_prevent_optimizer_from_removing_everything(foo);
		cout << "sort_using_less " << time << "musec" << endl;

	}

	{
		std::default_random_engine rng(42);
		unsigned node_count = 100000, arc_count = node_count*10;
		std::uniform_int_distribution<int> uniform_dist(0, node_count-1);

		vector<unsigned>tail(arc_count), head(arc_count);
		for(unsigned i=0; i<arc_count; ++i){
			tail[i] = uniform_dist(rng);
			head[i] = uniform_dist(rng);
		}

		vector<unsigned>sorted1_tail = tail, sorted1_head = head;

		{
			long long time = -get_micro_time();

			auto p = compute_inverse_stable_sort_permutation_using_key(sorted1_head, node_count, [](unsigned x){return x;});
			//inplace_apply_inverse_permutation(p, sorted1_tail);
			sorted1_tail = apply_inverse_permutation(p, sorted1_tail);
			auto q = compute_inverse_stable_sort_permutation_using_key(sorted1_tail, node_count, [](unsigned x){return x;});
			//inplace_apply_inverse_permutation(q, sorted1_tail);
			sorted1_tail = apply_inverse_permutation(q, sorted1_tail);
			p = chain_permutation_first_left_then_right(q, p);
			//inplace_apply_inverse_permutation(p, sorted1_head);
			sorted1_head = apply_inverse_permutation(p, sorted1_head);

			time += get_micro_time();

			cout << "Variant 1 needs "<< time << "musec" << endl;

			EXPECT(is_sorted_using_less(sorted1_tail));
		}

		vector<unsigned>sorted2_tail = tail, sorted2_head = head;

		{
			long long time = -get_micro_time();

			auto p = identity_permutation(arc_count);
			p = stable_sort_using_key(p, node_count, [&](unsigned x){return sorted2_head[x];});
			p = stable_sort_using_key(p, node_count, [&](unsigned x){return sorted2_tail[x];});

			p = invert_permutation(p);
			//inplace_apply_inverse_permutation(p, sorted2_tail);
			sorted2_tail = apply_inverse_permutation(p, sorted2_tail);
			//inplace_apply_inverse_permutation(p, sorted2_head);
			sorted2_head = apply_inverse_permutation(p, sorted2_head);


			time += get_micro_time();

			cout << "Variant 2 needs "<< time << "musec" << endl;

			EXPECT(is_sorted_using_less(sorted2_tail));
		}

		vector<unsigned>sorted3_tail = tail, sorted3_head = head;

		{
			long long time = -get_micro_time();

			auto p = identity_permutation(arc_count);
			p = stable_sort_using_key(p, node_count, [&](unsigned x){return sorted3_head[x];});
			p = stable_sort_using_key(p, node_count, [&](unsigned x){return sorted3_tail[x];});

			sorted3_tail = apply_permutation(p, sorted3_tail);
			sorted3_head = apply_permutation(p, sorted3_head);

			time += get_micro_time();

			cout << "Variant 3 needs "<< time << "musec" << endl;

			EXPECT(is_sorted_using_less(sorted3_tail));
		}

		EXPECT(sorted1_tail == sorted2_tail);
		EXPECT(sorted1_head == sorted2_head);

		EXPECT(sorted1_tail == sorted3_tail);
		EXPECT(sorted1_head == sorted3_head);

	}
	return expect_failed;
}
