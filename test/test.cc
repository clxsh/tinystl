#include "deque.h"
#include <iostream>
// using namespace std;
using namespace mystl;

struct test_struct {
public:
    int a;
    double b;
};

class test_class {
public:
    int c;
    double d;
    test_class() : c(0), d(0) {}
    test_class(int a, double b) : c(a), d(b) {}
};

int main()
{
    deque<test_class> td;
    td.push_back(test_class{1, 2.0});
    td.push_front(test_class{3, 4.0});

    std::cout << td.front().c << std::endl;
    std::cout << td.front().d << std::endl;

    return 0;
}