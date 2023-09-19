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
uint32x4_t shuffle_u32(uint32x4_t vec, int flag) {
    //uint32_t x[4];
   uint32_t a = vgetq_lane_u32(vec, 0);
    uint32_t b = vgetq_lane_u32(vec, 1);
    uint32_t c = vgetq_lane_u32(vec, 2);
    uint32_t d = vgetq_lane_u32(vec, 3);

    uint32x4_t value;
    value[0] = flag & 0x3;
    value[1] = (flag >> 2) & 0x3;
    value[2] = (flag >> 4) & 0x3;
    value[3] = (flag >> 6) & 0x3;

    uint32_t temp[4];
    temp[0] = (value[0] == 3) ? d : ((value[0] == 2) ? c : ((value[0] == 1) ? b : a));
    temp[1] = (value[1] == 3) ? d : ((value[1] == 2) ? c : ((value[1] == 1) ? b : a));
    temp[2] = (value[2] == 3) ? d : ((value[2] == 2) ? c : ((value[2] == 1) ? b : a));
    temp[3] = (value[3] == 3) ? d : ((value[3] == 2) ? c : ((value[3] == 1) ? b : a));
    //temp[0]=vgetq_lane_u32(vec,value[0]);

    

    return vld1q_u32(temp);
}
int main() {
    uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    uint32x4_t vec = vld1q_u32(state);
int flag = 0x0E;

printVector(vec, "vec");

uint32x4_t result = shuffle_u32(vec, flag);

printVector(result, "Result");

}