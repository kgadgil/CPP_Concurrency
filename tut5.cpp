#include <iostream>
#include <thread>
#include <mutex>

class bar {
public:
  void foo() {
    std::cout << "hello from member function" << std::endl;
  }
};

int main()
{
  std::thread t(&bar::foo, bar());
  t.join();
}