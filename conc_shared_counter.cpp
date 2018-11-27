#include <iostream>
#include <thread>
#include <mutex>

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
		mutex.lock();
		counter.increment();
		mutex.unlock();
	}

	void decrement(){
		mutex.lock();
		try{
			counter.decrement();	
		} catch (std::string e){
			mutex.unlock();
			throw e;
		}
		mutex.unlock();
	}
};

int main(int argc, char* argv[]){
	int NUMTHREADS = std::stoi(argv[1]);
	std::cout << "num threads " << NUMTHREADS << std::endl;
	Counter counter;
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

	std::cout << counter.value << std::endl;

	return 0;
}