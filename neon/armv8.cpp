#include <iostream>

int main() {
#ifdef __aarch64__
    std::cout << "ARMv8 architecture" << std::endl;
#else
    std::cout << "Not ARMv8 architecture" << std::endl;
#endif
    return 0;
}
