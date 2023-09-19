#include <iostream>
#include <arm_neon.h>

uint8x16_t neon_shuffle_epi8(uint8x16_t vec, uint8x16_t mask) {
    static const uint8_t lookup_table[16] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
    };

    return vreinterpretq_u8_u32(vtbl1q_u8(vreinterpretq_u8_u32(vec), vtbl1q_u8(vreinterpretq_u8_u32(vld1q_u32(lookup_table)), mask)));
}

int main() {
    uint8_t data[16] = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    uint8_t mask[16] = {0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8x16_t vec = vld1q_u8(data);
    uint8x16_t vec_mask = vld1q_u8(mask);

    uint8x16_t shuffled_epi8 = neon_shuffle_epi8(vec, vec_mask);

    std::cout << "Original (epi8): ";
    uint8_t* p_data = reinterpret_cast<uint8_t*>(&vec);
    for (int i = 0; i < 16; ++i) {
        std::cout << static_cast<int>(p_data[i]) << " ";
    }
    std::cout << std::endl;

    std::cout << "Shuffled (epi8): ";
    uint8_t* p_shuffled = reinterpret_cast<uint8_t*>(&shuffled_epi8);
    for (int i = 0; i < 16; ++i) {
        std::cout << static_cast<int>(p_shuffled[i]) << " ";
    }
    std::cout << std::endl;

    return 0;
}
