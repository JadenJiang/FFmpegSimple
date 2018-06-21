#include "test.h"
#include <iostream>
#include <thread>
int64_t factorial(int begin, int end) {
    int64_t total = 1;
    for (int i = begin; i > end; --i) {
        total *= i;
    }
    return total;
}

int64_t test(int row, int colum) {
    if (row - colum < colum - 1)
        colum = row - colum;
    return factorial(row - 1, (row - 1) - (colum - 1)) / factorial(colum - 1, 0);
}


int main() {
    for (int i = 1; i < 20; i++) {
        for (int j = 1; j <= i; j++) {
            std::cout << test(i, j) << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << test(7, 4);

    std::cout << std::endl;
	return 0;
}