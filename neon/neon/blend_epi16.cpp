#include <arm_neon.h>
#include <iostream>

void printVector(const uint16x8_t& vec, const char* name) {
    uint16_t values[8];
    vst1q_u16(values, vec);
    std::cout << name << ": ";
    for (int i = 0; i < 8; i++) {
        std::cout << values[i] << " ";
    }
    std::cout << std::endl;
}

uint16x8_t blend_epi16(const uint16x8_t& vec1, const uint16x8_t& vec2, const uint8x8_t& mask) {
    uint16x8_t result;
    uint16x8_t maskVec = vmovl_u8(mask);
    result = vandq_u16(vec1, vmvnq_u16(maskVec));
    result = vorrq_u16(result, vandq_u16(vec2, maskVec));
    return result;
}

int main() {
    uint16_t values1[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint16_t values2[8] = {11, 12, 13, 14, 15, 16, 17, 18};
    uint8_t maskValues[8] = {0, 1, 0, 1, 0, 1, 0, 1};

    uint16x8_t vec1 = vld1q_u16(values1);
    uint16x8_t vec2 = vld1q_u16(values2);
    uint8x8_t mask = vld1_u8(maskValues);

    printVector(vec1, "vec1");
    printVector(vec2, "vec2");
    printVector(mask, "mask");

    uint16x8_t result = blend_epi16(vec1, vec2, mask);

    printVector(result, "Result");

    return 0;
}
