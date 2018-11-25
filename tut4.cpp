#include <iostream>
#include <thread>
#include <initializer_list>
#include <vector>
#include <mutex>
#include <chrono>
#include <condition_variable>

//bool var = true;

void threadFunc (std::mutex &mtx, std::condition_variable &convar){
	std::unique_lock<std::mutex> lk(mtx);
	convar.wait(lk);							//convar acquires lock but wait means it unlocks
												//convar reacquires lock after notify_all
	/***************************************/
	//DONT DO THIS
	/***************************************/
	/*while(var){


	}*/
	
	std::cout << "end of thread" << std::endl;
}
int main (int argc, char* argv[]){
	std::mutex mtx;
	std::condition_variable convar;
	std::thread th{threadFunc, ref(mtx), ref(convar)};
	std::this_thread::sleep_for(std::chrono::seconds(10));

	//var=false;
	{
		std::lock_guard<std::mutex> lg(mtx);		//acquires lock
		convar.notify_all();						//notifies all instances of convar
	}
	
	th.join();
	std::cout << "end of code" << std::endl;
	return 0;
}
