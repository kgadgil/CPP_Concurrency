/*
Code from https://baptiste-wicht.com/posts/2012/04/c11-concurrency-tutorial-advanced-locking-and-condition-variables.html
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

struct Counter {
	int value;
	Counter () : value(0){}

	void increment (){
		++value;
	}

	void decrement (){
		if (value == 0){
			throw "Value less than 0";
		}
		--value;
	}
};

struct ConcurrentCounter {
	std::mutex mutex;
	Counter counter;

	void increment(){
		std::lock_guard<std::mutex> guard(mutex);
		counter.increment();
	}

	void decrement(){
		std::lock_guard<std::mutex> guard(mutex);
		counter.decrement();
	}
};

struct AtomicCounter {
	std::atomic<int> value;

	void increment(){
		++value;
	}

	void decrement(){
		--value;
	}

	int get(){
		return value.load();
	}
};

int main(int argc, char* argv[]){
	int NUMTHREADS = std::stoi(argv[1]);
	std::cout << "num threads " << NUMTHREADS << std::endl;
	//ConcurrentCounter counter;
	AtomicCounter counter;
	std::thread threadArr[NUMTHREADS];

	for (int i = 0; i < NUMTHREADS; ++i)
	{
		threadArr[i] = std::thread{[&counter](){
			for (int i = 0; i < 100; ++i)
			{
				counter.increment();
			}
		}};
	}

	for (int i = 0; i < NUMTHREADS; ++i)
	{
		threadArr[i].join();
	}

	//std::cout << counter.counter.value << std::endl;
	std::cout << counter.get() << std::endl;

	return 0;
}