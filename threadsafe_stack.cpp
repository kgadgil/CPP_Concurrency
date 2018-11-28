#include <iostream>
#include <exception>
#include <memory>
#include <thread>
#include <stack>
#include <mutex>
#include <algorithm>
#include <chrono>


struct empty_stack: std::exception {
	const char* what() const throw();
};

template<typename T> class threadsafe_stack {
private: 
	std::stack<T> data;
	mutable std::mutex m;
public:
		threadsafe_stack(){}
		threadsafe_stack(const threadsafe_stack& other){
			std::lock_guard<std::mutex> lock(other.m);
			data=other.data;
		}
		threadsafe_stack& operator = (const threadsafe_stack&) = delete;

		void push(T new_value){
			std::lock_guard<std::mutex> lock(m);
			data.push(std::move(new_value));
		}
		std::shared_ptr<T> pop(){
			std::lock_guard<std::mutex> lock(m);
			if(data.empty()) throw empty_stack();
			std::shared_ptr<T> const res(
				std::make_shared<T>(std::move(data.top())));
			data.pop();
			return res;
		}
		void pop(T& value){
			std::lock_guard<std::mutex> lock(m);
			if(data.empty()) throw empty_stack();
			value = std::move(data.top());
			data.pop();
		}
		bool empty() const{
			std::lock_guard<std::mutex> lock(m);
			return data.empty();
		}
};

/***************************
* Use Stack class in producer and consumer functions
***************************/

void consumer(int id, threadsafe_stack<int>& stack){
	std::cout << "consumer called" << std::endl;
	for (int i = 0; i < 10; ++i){
		std::cout << "before deq called" << std::endl;
		int value;
		stack.pop(value);
		std::cout << "Consumer " << id << " fetched" << value << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
}

void producer(int id, threadsafe_stack<int>& stack){
	std::cout << "producer called" << std::endl;
	for (int i = 0; i < 5; ++i){
		std::cout << "before enq called" << std::endl;
		stack.push(i);
		std::cout << "Produced " << id << " produced" << i << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
	}
}

int main (){
	threadsafe_stack<int> stack;

	std::thread c1(consumer, 0, std::ref(stack));
	std::thread p1(producer, 0, std::ref(stack));

	c1.join();
	p1.join();

	return 0;
}
