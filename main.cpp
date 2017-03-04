#include <iostream>
#include "any.h"
#include <any>

struct alignas(128) big {
    int field;

    big(int i) {
        field = i;
    }

    big(const big& other) {
        field = other.field;
    }

    big(big&& other) {
        field = other.field;
    }
};

int main() {
    // 1
    any a(3.1415);
    std::cout << any_cast<double>(a) << '\n';

    // 2
    any b(4.20);
    b = 1;
    std::cout << any_cast<int>(b) << '\n';

    a = 4;

    // 3
    big d(220);
    any c(d);
    std::cout << any_cast<big>(c).field << '\n';


    // 4
    for (int i = 0; i < 100000; i++) {
        big cc(i);
        any temp1(cc);
        any temp2(temp1);
        std::cout << any_cast<big>(temp2).field << '\n';

    }
    return 0;
}