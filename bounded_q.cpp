/*******
Herlihy-Shavitt's Bounded Partial Queue
******/

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <algorithm>
#include <condition_variable>
#include <chrono>

struct Node {
public:
	int value;
	Node *next;
};


struct BoundedQ {
	int capacity;
	std::atomic<int> size;
	std::mutex mutex;

	std::condition_variable not_full;
	std::condition_variable not_empty;

	Node *head, *tail;
	BoundedQ (int q_capacity) : capacity(q_capacity) {
		head = new Node;
		tail = new Node;
		head->value = 0;
		tail = head;
		size.store(0);
	}

	~BoundedQ(){
	}

	void enq(int x){
		bool mustWakeDequeuers = false;
		std::unique_lock<std::mutex> enqlock(mutex);
		while(size.load() == capacity){
			not_full.wait(enqlock);
		}
		Node *e = new Node;
		e->value = x;
		tail->next = e;
		tail = e;
		std::cout << "end size "<< size.load() << std::endl;
		if(size.fetch_add(1, std::memory_order_acq_rel) == 0){
			std::cout << "inend size "<< size.load() << std::endl;
			mustWakeDequeuers = true;
		}

		if (mustWakeDequeuers){
			std::cout << "wake deq" << mustWakeDequeuers << std::endl;
			not_empty.notify_all();		//convar is blocking so don't need lock surrounding it
		}
	}

	int deq(){
		//std::cout << "size of q "<< sizeQ() << std::endl;
		int result = 0;
		bool mustWakeEnqueuers = false;
		std::unique_lock<std::mutex> deqlock(mutex);
		while(size.load() == 0){
			not_empty.wait(deqlock);
		}
		Node *tmp = head->next;
		result = tmp->value;
		//std::cout << "removed node's value "<< tmp->value << std::endl;
		head = head->next;
		if (size.fetch_sub(1, std::memory_order_acq_rel) == capacity){
			mustWakeEnqueuers = true;
		}

		if(mustWakeEnqueuers){
			not_full.notify_all();
		}
		return result;
	}

	int sizeQ(){
		int sz = size.load();
		return sz;
	}
};


/***************************
* Use BoundedQ class in producer and consumer functions
***************************/

void consumer(BoundedQ& buffer, int size){
	std::cout << "consumer called" << std::endl;
//	auto start = std::chrono::system_clock::now();
	for (int i = 0; i < size; ++i){
		int value = buffer.deq();
		std::cout << "Consumer fetched" << value << std::endl;
	}
//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	std::cout << elapsed.count() << '\n';
}

void producer(BoundedQ& buffer, int size){
	std::cout << "producer called" << std::endl;
//	auto start = std::chrono::system_clock::now();
	for (int i = 0; i < size; ++i){
		buffer.enq(i);
		std::cout << "Produced produced" << i << std::endl;
	}
//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	std::cout << elapsed.count() << '\n';
}

int main (int argc, const char** argv){
	static const int BUFFER_SIZE = std::stoi(argv[1]);
	BoundedQ buffer(BUFFER_SIZE*sizeof(int));
	auto start = std::chrono::system_clock::now();
	std::thread p1(producer, std::ref(buffer), BUFFER_SIZE);
	std::thread c1(consumer, std::ref(buffer), BUFFER_SIZE);
	p1.join();
	c1.join();
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << elapsed.count() << '\n';
	std::cout << "size of q "<< buffer.sizeQ() << std::endl;
	return 0;
}
