#include <iostream>
#include <thread>

/*void threadFunction (int value) {
	std::cout << "inside thread function"<< std::endl;
	std::cout << "value is " << value << std::endl;
}*/

int main () {
	int value=10;
	/***************************************/
	//PASS FUNCTION TO THREAD IN TWO WAYS 
	/***************************************/
	//std::thread t1 (threadFunction)
	//std::thread t1 {threadFunction, value};

	/***************************************/
	//USE LAMBDA TO PASS FUNCTION TO THREAD (VALUE PASSED IN AS ARGUMENT BY REFERENCE)
	/***************************************/
	/*std::thread t1 {[&](){			//lambda can access local var by reference
		std::cout << "value in thread "<< ++value <<std::endl;
		std::cout << "value in thread "<< value++ <<std::endl;
	}};
	*/

	/***************************************/
	//USE LAMBDA TO PASS FUNCTION TO THREAD (VALUE PASSED IN AS ARGUMENT BY VALUE)
	/***************************************/
	std::thread t1 {[=](){			//lambda cannot change value passed in
		std::cout << "value in thread "<< value <<std::endl;
	}};

	t1.join();
	std::cout << "value in main thread "<< value<<std::endl;

	return 0;
}