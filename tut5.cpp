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

class DummyClass {
public:
    DummyClass()
    {}
    DummyClass(const DummyClass & obj)
    {}
    void sampleMemberFunction(int x)
    {
        std::cout<<"Inside sampleMemberFunction "<<x<<std::endl;
    }
};

template<typename T>
class TemplateClass {
public:
    TemplateClass()
    {}
    TemplateClass(const DummyClass & obj)
    {}
    void sampleMemberFunction(int x)
    {
        std::cout<<"Inside sampleMemberFunction "<<x<<std::endl;
    }
};

int main()
{
	std::vector<int> vec = {1,2,3};
  	std::thread t(&bar::foo, bar(), ref(vec));
	t.join();

	DummyClass dummyObj;
	int x = 10;
	std::thread threadObj(&DummyClass::sampleMemberFunction, &dummyObj, x);
	threadObj.join();

  TemplateClass<int> tempObj;
  std::thread t2(&TemplateClass<int>::sampleMemberFunction, std::ref(tempObj), x);
  t2.join();

  TemplateClass<double> tmp;
  std::thread t3([&]{
    tmp.sampleMemberFunction(x);
  });
  t3.join();

	return 0;
}