#include <iostream>
#include <thread>

void threadFunction () {
	std::cout << "inside thread function"<< std::endl;
}

int main () {
	int value=10;
	std::thread t1 {threadFunction};
	
	t1.join();

	return 0;
}