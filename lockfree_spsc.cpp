//Single-rpoducer single-consumer lock-free queue
//glitch-free code needed for audio programming

#include <iostream>
#include <thread>
#include <atomic>


template<typename T>
class lock_free_q {
private:
	struct Node{
		std::shared_ptr<T> value;
		Node* next;
		Node() : next(nullptr){} 
	};
	
	std::atomic<Node*> head;
	std::atomic<Node*> tail;
	tail.store(head);

	Node* deq_head(){
		Node* const old_head = head.load();
		if(old_head == tail.load()){
			return nullptr;
		}
		head.store(old_head->next);
		return old_head;
	}

public:
	lock_free_q (): head(new Node), tail(head.load()) 
	{}

	lock_free_q(const lock_free_q& other)=delete;
	lock_free_q& operator=(const lock_free_q& other)=delete;
	
	~lock_free_q(){
		while(Node* const old_head = head.load()){
			head.store(old_head->next);
			delete old_head;
		}
	}

	std::shared_ptr<T> deq(){
		Node* old_head = deq_head();
		if(!old_head){		//nullptr is convertible to bool true
			return std::shared_ptr<T>();
		}
		std::shared_ptr<T> const res(old_head->value);
		delete old_head;
		return res;
	}

	void enq(T new_value){
		std::shared_ptr<T> new_data (std::make_shared<T>(new_value));
		Node* p = new Node;
		Node* const old_tail = tail.load();
		old_tail->value.swap(new_data);
		old_tail->next = p;
		tail.store(p);
	}
};

/***************************
* Use lock_free_q class in producer and consumer functions
***************************/

void consumer(lock_free_q<int>& q, int size){
	std::cout << "consumer called" << std::endl;
	for (int i = 0; i < size; ++i){
		//std::cout << "before deq called" << std::endl;
		std::shared_ptr<int> value = q.deq();
		std::cout << "Consumer fetched" << value << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
}

void producer(lock_free_q<int>& q, int size){
	std::cout << "producer called" << std::endl;
	for (int i = 0; i < size; ++i){
		//std::cout << "before enq called" << std::endl;
		q.enq(i);
		std::cout << "Produced produced" << i << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
	}
}

int main (int argc, const char** argv){
	static const int BUFFER_SIZE = std::stoi(argv[1]);;
	lock_free_q<int> q;
	auto start = std::chrono::system_clock::now();
	std::thread c1(consumer, std::ref(q), BUFFER_SIZE);
	std::thread p1(producer, std::ref(q), BUFFER_SIZE);
	c1.join();
	p1.join();
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << elapsed.count() << '\n';
	return 0;
}
