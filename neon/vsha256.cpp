#include <arm_neon.h>
#include <iostream>

int main() {
    // 输入数据
    uint32_t data[4] = {0x61626364, 0x62636465, 0x63646566, 0x64656667};
    // 中间状态数据
    uint32_t state[4] = {0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A};
    // 结果数据
    uint32_t result[4];

    // 执行 vsha256su1q_u32 操作
    uint32x4_t data_vec = vld1q_u32(data);
    uint32x4_t state_vec = vld1q_u32(state);
    uint32x4_t result_vec = vsha256su1q_u32(data_vec, state_vec, state_vec);

    // 将结果保存到数组中
    vst1q_u32(result, result_vec);

    // 打印结果
    std::cout << "Result: ";
    for (int i = 0; i < 4; ++i) {
        std::cout << std::hex << result[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
