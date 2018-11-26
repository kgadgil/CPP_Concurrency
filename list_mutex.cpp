/***************************************/
//Protecting a list with mutex
/***************************************/
#include <iostream>
#include <thread>
#include <list>
#include <mutex>
#include <algorithm>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int val) {
	std::lock_guard<std::mutex> guard(some_mutex);
	some_list.push_back(val);
	std::cout << "pushed value " << val << " to list" << std::endl;
	std::cout << "locked by thread" << std::this_thread::get_id() << std::endl;
}

void list_contains(int val){
	std::lock_guard<std::mutex> guard(some_mutex);
	bool found = std::find(some_list.begin(),some_list.end(), val) != some_list.end();
	if (found){
		std::cout << "found value " << val << std::endl;	
	}
	else {
		std::cout << "not found value " << val << std::endl;
	}
	std::cout << "locked by thread" << std::this_thread::get_id() << std::endl;
}


int main (int argc, char* argv[]){
	int NUMTHREADS = std::stoi(argv[1]);
	std::cout << "num threads " << NUMTHREADS << std::endl;
	std::thread threadArr[NUMTHREADS];

	for (int i =0; i < NUMTHREADS; i++){
		threadArr[i] = std::thread{add_to_list, i};
	}
	for (int i =0; i < NUMTHREADS; i++){
		threadArr[i].join();
	}
}