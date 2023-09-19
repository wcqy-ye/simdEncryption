#include <arm_neon.h>
#include <iostream>
#include <iomanip> // 添加此头文件以使用 std::hex

void printVector(const uint32x4_t& vec, const char* name) {
    uint32_t values[4];
    vst1q_u32(values, vec);
    std::cout << name << ": ";
    for (int i = 0; i < 4; i++) {
        std::cout << std::hex << std::setw(8) << std::setfill('0') << values[i] << " ";
    }
    std::cout << std::dec << std::endl; // 切换回十进制输出
}

int main() {
    uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    //uint32x4_t vec1 = {1, 2, 3, 4};
    //uint32x4_t vec2 = {5, 6, 7, 8};
    uint32x4_t vec1 = vld1q_u32(state);
    uint32x4_t vec2 = vld1q_u32(state+4);

    printVector(vec1, "vec1");
    printVector(vec2, "vec2");

    uint32x4_t result = vextq_u32(vec1, vec2, 2);

    printVector(result, "Result");

    return 0;
}
