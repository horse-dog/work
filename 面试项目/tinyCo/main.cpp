#include "generator.hpp"
#include <iostream>

using namespace std;

Generator<int> fibonacci() {
    co_yield 0; // fib(0)
    co_yield 1; // fib(1)

    int a = 0;
    int b = 1;
    while(true) {
        co_yield a + b; // fib(N), N > 1
        b = a + b;
        a = b - a;
    }
}

int main(int argc, const char* argv[]) {
    
    auto generator = fibonacci();
    for (int i = 0; i < 10; i++) {
        if (generator.has_next()) {
            cout << generator.next() << endl;
        } else {
            break;
        }
    }
    return 0;

}