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
	size_t count;
	size_t capacity;
	std::condition_variable notFull notEmpty;
	public : 
		List_Q(size_t capacity=0): count(0), capacity(capacity);

		void enq (T x) {
			std::list <T> tmp;
			tmp.push_back(std::move(x));
			{
				std::lock_guard<std::mutex> lock(mutex_);
				q_.splice(end(q_), tmp);			//since we are inside lock, we want to do minimum work, finish fast. splice is constant time.
			}
		}

		T deq (){
			std::list <T> tmp;
			{
				std::lock_guard<std::mutex> lock(mutex_);
				tmp.splice(tmp.begin(), list, list.begin());
			}
		}
};



int main (int argc, char* argv[]){
	List_Q<int> q;
	std::thread t(&concurrent_queue<int>::enq, std::ref(q), 1);
	t.join();
}
