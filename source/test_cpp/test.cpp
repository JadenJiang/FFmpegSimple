#include "test.h"
#include <iostream>
#include <thread>
class Test {
public:
    Test() {
        printf("Test()\n");
    }
    ~Test() {
        printf("~Test()\n");
    }
};
int main() {
    std::unique_ptr<Test> a;
    a = std::unique_ptr<Test>(new Test());
	std::cout << "1111\n";
	return 0;
}