#include <iostream>
#include <thread>
#include <initializer_list>
#include <vector>
#include <mutex>
#include <chrono>

void threadFunc (std::mutex &mtx) {
	std::lock_guard<std::mutex> lock(mtx);			//resource acquired and released when the function goes out of scope
	std::cout << "locked by thread" << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(3));
}
int main (int argc, char* argv[]){
	int NUMTHREADS = std::stoi(argv[1]);
	std::cout << "num threads " << NUMTHREADS << std::endl;
	std::mutex mtx;

	std::thread threadArr[NUMTHREADS];

	for (int i =0; i < NUMTHREADS; i++){
		threadArr[i] = std::thread{threadFunc, ref(mtx)};
	}
	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::unique_lock<std::mutex> lock(mtx);			//unique_lock provides lock and unlock interfaces
	std::cout << "locked by main thread" << std::endl;
	lock.unlock();
	for (int i =0; i < NUMTHREADS; i++){
		threadArr[i].join();
	}
}