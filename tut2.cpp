#include <iostream>
#include <thread>

template<typename Type>
void threadFunc () {
	std::cout << "Type is => " << typeid(Type).name() <<std::endl;
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

	threadArr[0] = std::thread {threadFunc<int>};
	threadArr[1] = std::thread {threadFunc<float>};

	for (int i =0; i < NUMTHREADS; i++){
		threadArr[i].join();
	}
	return 0;
}