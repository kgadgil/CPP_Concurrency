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

template<typename T>
class concurrent_queue {
	std::mutex mutex_;
	std::list<T> q_;
	public : 
		void enq (T x) {
			std::list <T> tmp;
			tmp.push_back(std::move(x));
			{
				std::lock_guard<std::mutex> lock(mutex_);
				q_.splice(end(q_), tmp);
			}
		}
};



int main (int argc, char* argv[]){
	concurrent_queue<int> q;
	std::thread t(&concurrent_queue<int>::enq, std::ref(q), 1);
	t.join();
}
