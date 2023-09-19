#include <arm_neon.h>
#include <iostream>

void printVector(const uint8x16_t& vec, const char* name) {
    uint8_t values[16];
    vst1q_u8(values, vec);
    std::cout << name << ": ";
    for (int i = 0; i < 16; i++) {
        std::cout << static_cast<int>(values[i]) << " ";
    }
    std::cout << std::endl;
}

uint8x16_t alignr_epi8(const uint8x16_t& vec1, const uint8x16_t& vec2, int n) {
    return vextq_u8(vec1, vec2, n);
}


int main() {
    uint8_t values1[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint8_t values2[16] = {17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};

    uint8x16_t vec1 = vld1q_u8(values1);
    uint8x16_t vec2 = vld1q_u8(values2);

    printVector(vec1, "vec1");
    printVector(vec2, "vec2");

    int n = 8;  // Shift 8 elements to the right

    uint8x16_t result = alignr_epi8(vec1, vec2, n);

    printVector(result, "Result");

    return 0;
}
