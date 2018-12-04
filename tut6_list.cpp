#include <iostream>
#include <list>

void printlist (std::list<int> l_){
	std::list<int>::iterator it;
	for (it=l_.begin(); it!=l_.end(); ++it){
		std::cout << ' ' << *it;	
	}
    std::cout << std::endl;
}
int main (){
	std::list<int> l1 = {1,2,3};
	std::list<int> l2;
	printlist(l1);
	l2.splice(l2.begin(), l1, l1.begin());
	printlist(l2);
	printlist(l1);
	return 0;
}