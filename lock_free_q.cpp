#include <iostream>
#include <thread>
#include <atomic>

template<typename T>
class lock_free_q {
	struct Node{
		std::shared_ptr<T> value;
		Node* next;
		Node() : next(nullptr){} 
	};
	
	std::atomic<Node*> head, tail;
public:
	lock_free_q () {
		Node* node = new Node();
		head = new Node();
		tail = new Node();
		head.store(node);
		tail.store(node);
	}

	~lock_free_q(){
	}
	void enq(T const& value_){
		Node* const newnode = new Node(value_);
		while(true){
			Node* last = new Node();
			Node* next = new Node();
			last = tail.load();
			next = tail->next.load();
			if(last == tail.load()){
				if(next == NULL){
					//compare weak and strong; read api ref; weak used coz used in eg in api
					if(last->next.compare_exchange_weak(next, newnode, std::memory_order_release, std::memory_order_relaxed)){
						tail.compare_exchange_weak(last, newnode, std::memory_order_release, std::memory_order_relaxed);
						return;
					}
					else {
						tail.compare_exchange_weak(last, next, std::memory_order_release, std::memory_order_relaxed);
					}
				}
			}
		}
	}

	T deq(){
		while(true){
			Node* first = new Node();
			Node* last = new Node();
			Node* next = new Node();
			first = head.load();
			last = tail.load();
			next = first->next;

			if(first == head.load()){
				if (first == last){
					if(next == NULL){
						throw "Empty Exception";
					}
					tail.compare_exchange_weak(last, next, std::memory_order_release, std::memory_order_relaxed);
				}else{
					T value = next->value;
					if(head.compare_exchange_weak(first, next, std::memory_order_release, std::memory_order_relaxed)){
						return value;
					}
				}
			}
		}
	}

};

/***************************
* Use lock_free_q class in producer and consumer functions
***************************/

void consumer(int id, lock_free_q<int>& q){
	std::cout << "consumer called" << std::endl;
	for (int i = 0; i < 5; ++i){
		//std::cout << "before deq called" << std::endl;
		int value = q.deq();
		std::cout << "Consumer fetched" << value << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
}

void producer(int id, lock_free_q<int>& q){
	std::cout << "producer called" << std::endl;
	for (int i = 0; i < 10; ++i){
		//std::cout << "before enq called" << std::endl;
		q.enq(i);
		std::cout << "Produced produced" << i << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
	}
}

int main (int argc, const char** argv){
	lock_free_q<int> q;

	std::thread c1(consumer, 0, std::ref(q));
	std::thread p1(producer, 0, std::ref(q));
	c1.join();
	p1.join();
	return 0;
}
