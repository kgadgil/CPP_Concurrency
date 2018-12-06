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
#include <string>
#include <future>

struct Node {
public:
	double value;
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
		//std::cout << "constructor called" << std::endl;
	}

	BoundedQ(const BoundedQ& other)=delete;
	BoundedQ& operator = (const BoundedQ& other) = delete;	
	~BoundedQ(){
		while(Node* const tmp = head){
			head = tmp->next;
			delete tmp;
		}
		//std::cout << "destructor called" << std::endl;
	}

	void enq(double x){
		bool mustWakeDequeuers = false;
		std::unique_lock<std::mutex> enqlock(mutex);
		//std::cout << "enqlock acquired" << std::endl;
		while(size.load() == capacity){
			//std::cout << "waiting for not_full" << std::endl;
			not_full.wait(enqlock);
		}
		Node *e = new Node;
		e->value = x;
		tail->next = e;
		tail = e;
		//std::cout << "end size "<< size.load() << std::endl;
		if(size.fetch_add(1, std::memory_order_acq_rel) == 0){
			//std::cout << "inend size "<< size.load() << std::endl;
			mustWakeDequeuers = true;
		}
		enqlock.unlock();
		//std::cout << "enq unlock" << std::endl;
		if (mustWakeDequeuers){
			std::lock_guard<std::mutex> lk(mutex);
			//std::cout << "lock acq; update not_empty" << std::endl;
			not_empty.notify_all();		//convar is blocking so don't need lock surrounding it
		}
	}

	int deq(){
		//std::cout << "size of q "<< sizeQ() << std::endl;
		double result = 0;
		bool mustWakeEnqueuers = false;
		std::unique_lock<std::mutex> deqlock(mutex);
		//std::cout << "deqlock acquired" << std::endl;
		while(size.load() == 0){
			//std::cout << "waiting for not_empty" << std::endl;
			not_empty.wait(deqlock);
		}
		Node *tmp = head->next;
		result = tmp->value;
		//std::cout << "removed node's value "<< tmp->value << std::endl;
		head = head->next;
		if (size.fetch_sub(1, std::memory_order_acq_rel) == capacity){
			mustWakeEnqueuers = true;
		}
		deqlock.unlock();
		//std::cout << "deq unlock" << std::endl;
		if(mustWakeEnqueuers){
			std::lock_guard<std::mutex> lk(mutex);
			//std::cout << "lock acq; update not_full" << std::endl;
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

void consumer(BoundedQ& buffer, int ncons_items, std::future<void>&& fut, int delay){
	fut.wait();
	for (int i = 0; i < ncons_items; ++i){
		std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
		int value = buffer.deq();
	}
}

void producer(BoundedQ& buffer, int nprod_items, std::promise<void>&& prom, int delay){
	for (int i = 0; i < nprod_items; ++i){
		double r = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/5));
		std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
		buffer.enq(r);
		if (i == ((nprod_items*3)/4)){
			prom.set_value();
		}
	}
}

int main (int argc, char* argv[]){
	int BUFFER_SIZE = std::stoi(argv[1]);
	int PROD_DELAY = std::stoi(argv[2]);
	int CONSUME_DELAY = std::stoi(argv[3]);
	int buffer_capacity = BUFFER_SIZE*sizeof(double);

	BoundedQ buffer(buffer_capacity);
	std::promise<void> data_ready;
	auto fut = data_ready.get_future();

	auto start = std::chrono::system_clock::now();
	std::thread p1(producer, std::ref(buffer), BUFFER_SIZE, std::move(data_ready), PROD_DELAY);
	std::thread c1(consumer, std::ref(buffer), BUFFER_SIZE, std::move(fut), CONSUME_DELAY);
	c1.join();
	p1.join();
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	std::cout << elapsed.count() << '\n';
	//std::cout << "size of q "<< buffer.sizeQ() << std::endl;
	return 0;
}
