#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

class bar {
public:
  void foo(std::vector<int> &vec) {
  	for (std::vector<int>::const_iterator i = vec.begin(); i != vec.end(); ++i){
    	std::cout << *i << ' ';
	}std::cout << std::endl;
  }
};

int main()
{
	std::vector<int> vec = {1,2,3};
  	std::thread t(&bar::foo, bar(), ref(vec));
	t.join();
}