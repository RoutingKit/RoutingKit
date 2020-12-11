#include <routingkit/id_set_queue.h>

#include "expect.h"

#include <iostream>

using namespace RoutingKit;
using namespace std;

int main(){
	try{
		IDSetMinQueue q(100);
		EXPECT_CMP(q.id_count(), ==, 100);

		EXPECT(q.empty());

		q.push(3);
		q.push(8);
		q.push(77);
		q.push(2);
		q.push(15);
		q.push(66);

		EXPECT(q.contains(77));
		EXPECT(!q.contains(78));

		EXPECT_CMP(q.peek(), ==, 2); EXPECT_CMP(q.pop(), ==, 2);
		EXPECT_CMP(q.peek(), ==, 3); EXPECT_CMP(q.pop(), ==, 3);
		EXPECT_CMP(q.peek(), ==, 8); EXPECT_CMP(q.pop(), ==, 8);
		EXPECT_CMP(q.peek(), ==, 15); EXPECT_CMP(q.pop(), ==, 15);
		EXPECT_CMP(q.peek(), ==, 66); EXPECT_CMP(q.pop(), ==, 66);
		EXPECT_CMP(q.peek(), ==, 77); EXPECT_CMP(q.pop(), ==, 77);

		EXPECT(q.empty());

		q.push(77);
		q.push(2);
		q.push(15);

		EXPECT_CMP(q.peek(), ==, 2); EXPECT_CMP(q.pop(), ==, 2);
		EXPECT_CMP(q.peek(), ==, 15); EXPECT_CMP(q.pop(), ==, 15);
		
		q.push(3);
		q.push(8);
		
		EXPECT_CMP(q.peek(), ==, 3); EXPECT_CMP(q.pop(), ==, 3);
		EXPECT_CMP(q.peek(), ==, 8); EXPECT_CMP(q.pop(), ==, 8);
		
		q.clear();

		EXPECT(q.empty());

		q = IDSetMinQueue(5);
		q.push(4);
		q.pop();

		cout << "All finished" << endl;
	}catch(std::exception&err){
		cout << "exception" << ":" << err.what() << endl;
		return 1;
	}
	return expect_failed;
}
