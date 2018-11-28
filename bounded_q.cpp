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
	int count;
	int capacity;
	std::atomic<int> size;
	std::mutex mutex;

	std::condition_variable not_full;
	std::condition_variable not_empty;

	Node *head, *tail;
	BoundedQ (int capacity) : capacity(capacity), count(0) {
		head = NULL;
		tail = NULL;
		size.store(0);
	}

	~BoundedQ(){
	}

	void enq(int x){
		std::cout << "inside enq" << std::endl;
		bool mustWakeDequeuers = false;
		std::unique_lock<std::mutex> enqlock(mutex);
		std::cout << "enqlocked" << std::endl;
		while(size.load() == capacity){
			not_full.wait(
				enqlock, [this]() {
					return count != capacity;
				});
		}
		Node *e = new Node;
		e->value = x;
		tail->next = tail;
		tail = e;

		if(size.fetch_add(1, std::memory_order_acq_rel) == 0){
			mustWakeDequeuers = true;
		}

		if (mustWakeDequeuers){
			not_empty.notify_all();		//convar is blocking so don't need lock surrounding it
		}
	}

	int deq(){
		std::cout << "inside deq" << std::endl;
		int result;
		bool mustWakeEnqueuers = true;
		std::unique_lock<std::mutex> deqlock(mutex);
		std::cout << "enqlocked" << std::endl;
		while(size.load() == 0){
			not_empty.wait(
				deqlock, [this]() {
					return count != 0;
				});
		}
		Node *tmp = head->next;
		result = tmp->value;
		head = head->next;
		if (size.fetch_add(1, std::memory_order_acq_rel) == capacity){
			mustWakeEnqueuers = true;
		}

		if(mustWakeEnqueuers){
			not_full.notify_all();
		}
		return result;
	}
};


/***************************
* Use BoundedQ class in producer and consumer functions
***************************/

void consumer(int id, BoundedQ& buffer){
	std::cout << "consumer called" << std::endl;
	for (int i = 0; i < 50; ++i){
		std::cout << "before deq called" << std::endl;
		int value = buffer.deq();
		std::cout << "Consumer " << id << " fetched" << value << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
}

void producer(int id, BoundedQ& buffer){
	std::cout << "producer called" << std::endl;
	for (int i = 0; i < 75; ++i){
		std::cout << "before enq called" << std::endl;
		buffer.enq(i);
		std::cout << "Produced " << id << " produced" << i << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
	}
}

int main (){
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

	return 0;
}
