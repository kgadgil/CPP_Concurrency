/*************
Sean Parent's splice trick for conc queue: C++ Seasoning
std::list with splice can be used in a pinch to create a thread-safe data structure
it's a good crutch for when you don't want to use TBB or Boost
whip one up!
**************/

#include <iostream>
#include <thread>
#include <list>
#include <mutex>
#include <condition_variable>

template<typename T>
class List_Q {
	std::mutex mutex_;
	std::list<T> q_;
	public : 
		List_Q(std::list<T> queue): q_(queue){
			std::cout << "constructor" << std::endl;
		}

		void enq (T x) {
			std::list <T> tmp;
			tmp.push_back(std::move(x));
			{
				std::lock_guard<std::mutex> lock(mutex_);
				q_.splice(end(q_), tmp);			//since we are inside lock, we want to do minimum work, finish fast. splice is constant time.
			}
		}

		T deq (){
			if(q_.empty()){
				throw std::logic_error ("queue is empty");
			}
			std::list <T> tmp;
			{
				std::lock_guard<std::mutex> lock(mutex_);
				tmp.splice(tmp.begin(), q_, q_.begin());
			}
			std::cout << "deqed " << tmp.front() << std::endl;
			return tmp.front();
		}
};

void consumer(List_Q<int>& buffer, int size){
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

void producer(List_Q<int>& buffer, int size){
	std::cout << "producer called" << std::endl;
//	auto start = std::chrono::system_clock::now();
	for (int i = 0; i <size; ++i){
		int data = i;
		buffer.enq(data);
		std::cout << "Produced produced" << i << std::endl;
	}
//	auto end = std::chrono::system_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//	std::cout << elapsed.count() << '\n';
}

int main (int argc, const char** argv){
	static const int BUFFER_SIZE = std::stoi(argv[1]);
	std::list<int> newlist;
	List_Q<int> buffer(newlist);
	auto start = std::chrono::system_clock::now();
	std::thread p1([&]{
		producer(buffer, BUFFER_SIZE);
	});
	std::thread c1([&]{
		consumer(buffer, BUFFER_SIZE);
	});
	p1.join();
	c1.join();
	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << elapsed.count() << '\n';
	return 0;
}