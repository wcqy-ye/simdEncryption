#include <arm_neon.h>

int main() {
    uint32_t data[12] = { /* Initialize your data here */ };
    uint32x4_t tw0_3 = vld1q_u32(&data[0]);
    uint32x4_t w8_11 = vld1q_u32(&data[8]);
    uint32x4_t w12_15 = vld1q_u32(&data[12]);

    uint32x4_t result = __builtin_arm_crypto_sha256su1(tw0_3, w8_11, w12_15);

    // Do something with the result...

    return 0;
}
