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
		std::cout << "inside deq" << std::endl;
		int result = 0;
		bool mustWakeEnqueuers = false;
		std::unique_lock<std::mutex> deqlock(mutex);
		while(size.load() == 0){
			not_empty.wait(deqlock);
		}
		Node *tmp = head->next;
		result = tmp->value;
		std::cout << "removed node's value "<< tmp->value << std::endl;
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

void consumer(int id, BoundedQ& buffer){
	std::cout << "consumer called" << std::endl;
	for (int i = 0; i < 5; ++i){
		//std::cout << "before deq called" << std::endl;
		int value = buffer.deq();
		std::cout << "Consumer fetched" << value << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
}

void producer(int id, BoundedQ& buffer){
	std::cout << "producer called" << std::endl;
	for (int i = 0; i < 10; ++i){
		//std::cout << "before enq called" << std::endl;
		buffer.enq(i);
		std::cout << "Produced produced" << i << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
	}
}

int main (int argc, const char** argv){
	//unsigned num_cpus = std::thread::hardware_concurrency();
	//std::cout << "Launching " << num_cpus << " threads\n";
	BoundedQ buffer(10);

	std::thread c1(consumer, 0, std::ref(buffer));
	//std::thread c2(consumer, 1, std::ref(buffer));
	//std::thread c3(consumer, 2, std::ref(buffer));
	std::thread p1(producer, 0, std::ref(buffer));
	//std::thread p2(producer, 1, std::ref(buffer));
	c1.join();
	//c2.join();
	//c3.join();
	p1.join();
	//p2.join();
	std::cout << "size of q "<< buffer.sizeQ() << std::endl;
	return 0;
}
