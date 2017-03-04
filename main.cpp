#include <iostream>
#include "any.h"

struct alignas(128) big {
    int fields[32];
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
    big d{220};
    d.fields[0] = 220;
    any c(d);
    std::cout << any_cast<big>(c).fields[0] << '\n';

    for (int i = 0; i < 100000; i++) {
        int temp;
        temp = 111;
        any temp2(temp);
        std::cout << any_cast<int>(temp2) << '\n';

    }
    return 0;
}