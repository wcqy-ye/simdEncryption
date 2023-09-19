#include <stdint.h>
#include <stdio.h>
#include <arm_neon.h>

static inline uint32x4_t my_vsha256su1q_u32(uint32x4_t a, uint32x4_t b)
{
    return veorq_u32(veorq_u32(vshrq_n_u32(a, 17), vshrq_n_u32(a, 19)), vshrq_n_u32(a, 10));
}

static inline uint32x4_t my_vsha256sig0q_u32(uint32x4_t a)
{
    return veorq_u32(veorq_u32(vshrq_n_u32(a, 7), vshrq_n_u32(a, 18)), vshrq_n_u32(a, 3));
}

static inline uint32x4_t my_vsha256sig1q_u32(uint32x4_t a)
{
    return veorq_u32(veorq_u32(vshrq_n_u32(a, 17), vshrq_n_u32(a, 19)), vshrq_n_u32(a, 10));
}

void sha256_block(uint32_t state[8], const uint8_t message[])
{
    // Load the message schedule constants
    const uint32_t constant32[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    // Load the initial hash values
    uint32x4_t STATE0 = vld1q_u32(&state[0]);
    uint32x4_t STATE1 = vld1q_u32(&state[4]);

    // Load the message block
    uint32x4_t MSG0 = vreinterpretq_u32_u8(vld1q_u8(&message[0]));
    uint32x4_t MSG1 = vreinterpretq_u32_u8(vld1q_u8(&message[16]));
    uint32x4_t MSG2 = vreinterpretq_u32_u8(vld1q_u8(&message[32]));
    uint32x4_t MSG3 = vreinterpretq_u32_u8(vld1q_u8(&message[48]));

    // Rounds 0-3
    uint32x4_t TMP = MSG3;
    MSG3 = MSG2;
    MSG2 = MSG1;
    MSG1 = vaddq_u32(my_vsha256su1q_u32(MSG1, MSG0), my_vsha256sig1q_u32(MSG3));
    MSG0 = TMP;
    MSG0 = vaddq_u32(MSG0, vld1q_u32(&constant32[0]));
    STATE1 = vsha256h2q_u32(STATE1, STATE0, MSG0);
    TMP = MSG3;
    MSG3 = MSG2;
    MSG2 = MSG1;
    MSG1 = vaddq_u32(my_vsha256su0q_u32(MSG1, MSG0, MSG3), vsha256majq_u32(MSG1, MSG0, MSG3));
    MSG0 = TMP;
    MSG0 = vaddq_u32(MSG0, vld1q_u32(&constant32[1]));
    STATE1 = vsha256h1q_u32(STATE1, STATE0, MSG0);

    // Rounds 4-7
    TMP = MSG3;
    MSG3 = MSG2;
    MSG2 = MSG1;
    MSG1 = vaddq_u32(my_vsha256su1q_u32(MSG1, MSG0), my_vsha256sig0q_u32(MSG3));
    MSG0 = TMP;
    MSG0 = vaddq_u32(MSG0, vld1q_u32(&constant32[2]));
    STATE1 = vsha256h2q_u32(STATE1, STATE0, MSG0);
    TMP = MSG3;
    MSG3 = MSG2;
    MSG2 = MSG1;
    MSG1 = vaddq_u32(my_vsha256su0q_u32(MSG1, MSG0, MSG3), vsha256majq_u32(MSG1, MSG0, MSG3));
    MSG0 = TMP;
    MSG0 = vaddq_u32(MSG0, vld1q_u32(&constant32[3]));
    STATE1 = vsha256h1q_u32(STATE1, STATE0, MSG0);

    // Rounds 8-15
    for (int i = 0; i < 2; i++) {
        TMP = MSG3;
        MSG3 = MSG2;
        MSG2 = MSG1;
        MSG1 = vaddq_u32(my_vsha256su1q_u32(MSG1, MSG0), my_vsha256sig1q_u32(MSG3));
        MSG0 = TMP;
        MSG0 = vaddq_u32(MSG0, vld1q_u32(&constant32[4 + i * 4]));
        STATE1 = vsha256h2q_u32(STATE1, STATE0, MSG0);
        TMP = MSG3;
        MSG3 = MSG2;
        MSG2 = MSG1;
        MSG1 = vaddq_u32(my_vsha256su0q_u32(MSG1, MSG0, MSG3), vsha256majq_u32(MSG1, MSG0, MSG3));
        MSG0 = TMP;
        MSG0 = vaddq_u32(MSG0, vld1q_u32(&constant32[5 + i * 4]));
        STATE1 = vsha256h1q_u32(STATE1, STATE0, MSG0);
    }

    // Rounds 16-63
    for (int i = 0; i < 48; i++) {
        TMP = MSG3;
        MSG3 = MSG2;
        MSG2 = MSG1;
        MSG1 = vaddq_u32(my_vsha256su1q_u32(MSG1, MSG0), my_vsha256sig1q_u32(MSG3));
        MSG0 = TMP;
        MSG0 = vaddq_u32(MSG0, vld1q_u32(&constant32[4 + i * 4]));
        STATE1 = vsha256h2q_u32(STATE1, STATE0, MSG0);
        TMP = MSG3;
        MSG3 = MSG2;
        MSG2 = MSG1;
        MSG1 = vaddq_u32(my_vsha256su0q_u32(MSG1, MSG0, MSG3), vsha256majq_u32(MSG1, MSG0, MSG3));
        MSG0 = TMP;
        MSG0 = vaddq_u32(MSG0, vld1q_u32(&constant32[5 + i * 4]));
        STATE1 = vsha256h1q_u32(STATE1, STATE0, MSG0);
    }

    // Update the hash values
    STATE0 = vaddq_u32(STATE0, vld1q_u32(&state[0]));
    STATE1 = vaddq_u32(STATE1, vld1q_u32(&state[4]));
    vst1q_u32(&state[0], STATE0);
    vst1q_u32(&state[4], STATE1);
}

int main() {
    // Example usage
    uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    uint8_t message[64] = {0}; // Replace with your own message
    sha256_block(state, message);

    // Print the resulting hash
    for (int i = 0; i < 8; i++) {
        printf("%08x ", state[i]);
    }
    printf("\n");

    return 0;
}
