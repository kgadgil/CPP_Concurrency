#include <iostream>
#include <thread>
#include <initializer_list>
#include <vector>

template<typename T>
void printme (T&& t) {
	std::cout << "Type is => " << typeid(T).name() <<std::endl;
	for(auto i : t) {
		std::cout << i;
	}
	std::cout << std::endl;
}


int main (int argc, char* argv[]){
	int NUMTHREADS = std::stoi(argv[1]);
	std::cout << "num threads " << NUMTHREADS << std::endl;

	std::thread threadArr[NUMTHREADS];

	/***************************************/
	//LAMBDA FUNC TO PRINT THREADID OF ARRAY OF THREADS 
	/***************************************/
	/*for (int i = 0; i < NUMTHREADS; i++){
		threadArr[i] = std::thread{
			[&](){
				std::cout << "threadid " << std::this_thread::get_id() << std::endl;
			}
		};
	}*/


	/***************************************/
	//PASSING TEMPLATE FUNCTION 
	/***************************************/

	//threadArr[0] = std::thread {threadFunc<int>};
	//threadArr[1] = std::thread {threadFunc<float>};


	/***************************************/
	//INITIALIZER LIST 
	/***************************************/

	std::initializer_list<int> il = {1,2,3};
	std::vector<int> vec = {4,5,6};
	printme(il);
	printme(vec);

	/*for (int i =0; i < NUMTHREADS; i++){
		threadArr[i].join();
	}*/
	return 0;
}