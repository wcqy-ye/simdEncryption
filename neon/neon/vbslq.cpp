#include <arm_neon.h>
#include <iostream>
#include <stdio.h>
void blendExample()
{
    uint8x16_t mask = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    uint8x16_t vector1 = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160};
    uint8x16_t vector2 = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    uint8x16_t result = vbslq_u8(mask, vector1, vector2);

    // 输出结果
    uint8_t values[16];
    vst1q_u8(values, result);
    for (int i = 0; i < 16; i++)
    {
        printf("%u ", values[i]);
    }
    printf("\n");
}

int main()
{
    blendExample();
    return 0;
}
