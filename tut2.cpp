#include <iostream>
#include <thread>

int main (int argc, char* argv[]){
	int NUMTHREADS = std::stoi(argv[1]);
	std::cout << "num threads " << NUMTHREADS << std::endl;

	std::thread threadArr[NUMTHREADS];

	for (int i = 0; i < NUMTHREADS; i++){
		threadArr[i] = std::thread{
			[&](){
				std::cout << "threadid " << std::this_thread::get_id() << std::endl;
			}
		};
	}

	for (int i =0; i < NUMTHREADS; i++){
		threadArr[i].join();
	}
	return 0;
}