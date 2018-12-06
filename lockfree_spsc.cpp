//Single-rpoducer single-consumer lock-free queue
//glitch-free code needed for audio programming

#include <iostream>
#include <thread>
#include <atomic>
#include <future>


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

void consumer(lock_free_q<double>& q, int size, std::future<void>&& fut){
	fut.wait();
	for (int i = 0; i < size; ++i){
		std::shared_ptr<double> value = q.deq();
		//std::cout << "Consumer fetched" << value << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
}

void producer(lock_free_q<double>& q, int size, std::promise<void>&& prom){
	for (int i = 0; i < size; ++i){
		double r = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/5));
		q.enq(r);
		if (i == 2){
			std::cout << "producer wrote 2 items" << std::endl;
			prom.set_value();
		}
		//std::cout << "Produced produced" << i << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
	}
}

int main (int argc, const char** argv){
	static const int BUFFER_SIZE = std::stoi(argv[1]);;
	lock_free_q<double> q;
	std::promise<void> data_ready;
	auto fut = data_ready.get_future();

	auto start = std::chrono::system_clock::now();
	std::thread p1(producer, std::ref(q), BUFFER_SIZE*1000, std::move(data_ready));
	std::thread c1(consumer, std::ref(q), BUFFER_SIZE,std::move(fut));
	p1.join();
	c1.join();
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	std::cout << elapsed.count() << '\n';
	return 0;
}
