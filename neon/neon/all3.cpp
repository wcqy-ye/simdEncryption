#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arm_neon.h>
#include <iostream>
#include <iomanip>
#include <cstring>

typedef uint32_t word32;
// SHA-256算法中一个块的大小（以字节为单位）
#define SHA256_BLOCK_SIZE 64
enum ByteOrder { BIG_ENDIAN_ORDER, LITTLE_ENDIAN_ORDER };

typedef struct {
    word32 state[8];
    word32 buffer[SHA256_BLOCK_SIZE / sizeof(word32)];
    size_t buffer_len;
    word32 num_bits;
} SHA256_CTX;

const uint32_t SHA256_K[64] = {

    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// 循环左移和字节序转换宏
#define ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define SwapEndian32(x) __builtin_bswap32(x)

void SHA256_Init(SHA256_CTX *ctx) {
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
    ctx->buffer_len = 0;
    ctx->num_bits = 0;
}



void SHA256_HashMultipleBlocks_ARMV8(word32 *state, const word32 *data, size_t length, ByteOrder order)
{
    uint32x4_t STATE0, STATE1, ABEF_SAVE, CDGH_SAVE;
    uint32x4_t MSG0, MSG1, MSG2, MSG3;
    uint32x4_t TMP0, TMP1, TMP2;

    // Load initial values
    STATE0 = vld1q_u32(&state[0]);
    STATE1 = vld1q_u32(&state[4]);

    while (length >= SHA256_BLOCK_SIZE)
    {
        // Save current hash
        ABEF_SAVE = STATE0;
        CDGH_SAVE = STATE1;

        // Load message
        MSG0 = vld1q_u32(data +  0);
        MSG1 = vld1q_u32(data +  4);
        MSG2 = vld1q_u32(data +  8);
        MSG3 = vld1q_u32(data + 12);

        if (order == BIG_ENDIAN_ORDER)  // Data arrangement
        {
            MSG0 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG0)));
            MSG1 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG1)));
            MSG2 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG2)));
            MSG3 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG3)));
        }

        TMP0 = vaddq_u32(MSG0, vld1q_u32(&SHA256_K[0x00]));

        // Rounds 0-3
        MSG0 = vsha256su0q_u32(MSG0, MSG1);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG1, vld1q_u32(&SHA256_K[0x04]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG0 = vsha256su1q_u32(MSG0, MSG2, MSG3);

        // Rounds 4-7
        MSG1 = vsha256su0q_u32(MSG1, MSG2);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG2, vld1q_u32(&SHA256_K[0x08]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG1 = vsha256su1q_u32(MSG1, MSG3, MSG0);

        // Rounds 8-11
        MSG2 = vsha256su0q_u32(MSG2, MSG3);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG3, vld1q_u32(&SHA256_K[0x0c]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG2 = vsha256su1q_u32(MSG2, MSG0, MSG1);

        // Rounds 12-15
        MSG3 = vsha256su0q_u32(MSG3, MSG0);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG0, vld1q_u32(&SHA256_K[0x10]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG3 = vsha256su1q_u32(MSG3, MSG1, MSG2);

        // Rounds 16-19
        MSG0 = vsha256su0q_u32(MSG0, MSG1);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG1, vld1q_u32(&SHA256_K[0x14]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG0 = vsha256su1q_u32(MSG0, MSG2, MSG3);

        // Rounds 20-23
        MSG1 = vsha256su0q_u32(MSG1, MSG2);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG2, vld1q_u32(&SHA256_K[0x18]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG1 = vsha256su1q_u32(MSG1, MSG3, MSG0);

        // Rounds 24-27
        MSG2 = vsha256su0q_u32(MSG2, MSG3);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG3, vld1q_u32(&SHA256_K[0x1c]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG2 = vsha256su1q_u32(MSG2, MSG0, MSG1);

        // Rounds 28-31
        MSG3 = vsha256su0q_u32(MSG3, MSG0);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG0, vld1q_u32(&SHA256_K[0x20]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG3 = vsha256su1q_u32(MSG3, MSG1, MSG2);

        // Rounds 32-35
        MSG0 = vsha256su0q_u32(MSG0, MSG1);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG1, vld1q_u32(&SHA256_K[0x24]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG0 = vsha256su1q_u32(MSG0, MSG2, MSG3);

        // Rounds 36-39
        MSG1 = vsha256su0q_u32(MSG1, MSG2);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG2, vld1q_u32(&SHA256_K[0x28]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG1 = vsha256su1q_u32(MSG1, MSG3, MSG0);

        // Rounds 40-43
        MSG2 = vsha256su0q_u32(MSG2, MSG3);
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG3, vld1q_u32(&SHA256_K[0x2c]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);
        MSG2 = vsha256su1q_u32(MSG2, MSG0, MSG1);

        // Rounds 44-47
        MSG3 = vsha256su0q_u32(MSG3, MSG0);
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG0, vld1q_u32(&SHA256_K[0x30]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);
        MSG3 = vsha256su1q_u32(MSG3, MSG1, MSG2);

        // Rounds 48-51
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG1, vld1q_u32(&SHA256_K[0x34]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);

        // Rounds 52-55
        TMP2 = STATE0;
        TMP0 = vaddq_u32(MSG2, vld1q_u32(&SHA256_K[0x38]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);

        // Rounds 56-59
        TMP2 = STATE0;
        TMP1 = vaddq_u32(MSG3, vld1q_u32(&SHA256_K[0x3c]));
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP0);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP0);

        // Rounds 60-63
        TMP2 = STATE0;
        STATE0 = vsha256hq_u32(STATE0, STATE1, TMP1);
        STATE1 = vsha256h2q_u32(STATE1, TMP2, TMP1);

        // Add back to state
        STATE0 = vaddq_u32(STATE0, ABEF_SAVE);
        STATE1 = vaddq_u32(STATE1, CDGH_SAVE);

        data += SHA256_BLOCK_SIZE/sizeof(word32);
        length -= SHA256_BLOCK_SIZE;
    }

    // Save state
    vst1q_u32(&state[0], STATE0);
    vst1q_u32(&state[4], STATE1);
}


void SHA256_Final(SHA256_CTX *ctx, uint8_t hash[32]) {
    // 填充最后一个块
    size_t last_block_size = ctx->buffer_len;
    size_t padded_size = (last_block_size < SHA256_BLOCK_SIZE - sizeof(word32)) ? SHA256_BLOCK_SIZE : 2 * SHA256_BLOCK_SIZE;
    uint8_t *padded_block = (uint8_t *)malloc(padded_size);

    memset(padded_block, 0, padded_size);
    memcpy(padded_block, ctx->buffer, last_block_size);
    padded_block[last_block_size] = 0x80;

    // 保存总比特数
    word32 total_bits = ctx->num_bits;
    if (ctx->buffer_len >= SHA256_BLOCK_SIZE - sizeof(word32))
        total_bits += 8 * SHA256_BLOCK_SIZE;

    total_bits = SwapEndian32(total_bits);
    memcpy(padded_block + padded_size - sizeof(word32), &total_bits, sizeof(word32));

    // 计算SHA-256最终哈希值
    for (size_t i = 0; i < padded_size; i += SHA256_BLOCK_SIZE) {
        SHA256_HashMultipleBlocks_ARMV8(ctx->state, (const word32 *)(padded_block + i), SHA256_BLOCK_SIZE, BIG_ENDIAN_ORDER);
    }

    free(padded_block);

    // 输出最终哈希值
    for (int i = 0; i < 8; i++) {
        hash[i * 4] = (ctx->state[i] >> 24) & 0xFF;
        hash[i * 4 + 1] = (ctx->state[i] >> 16) & 0xFF;
        hash[i * 4 + 2] = (ctx->state[i] >> 8) & 0xFF;
        hash[i * 4 + 3] = ctx->state[i] & 0xFF;
    }
}
int main() {
    SHA256_CTX ctx;
    uint8_t hash[32];

    // 初始化SHA-256上下文
    SHA256_Init(&ctx);

    // 准备输入数据 "Hello, world!"
    const char* input_data = "Hello, world!";
    size_t data_len = strlen(input_data);

    // 处理所有输入数据块
    while (data_len >= SHA256_BLOCK_SIZE) {
        // 将数据块传递给循环处理函数
        SHA256_HashMultipleBlocks_ARMV8(ctx.state, (const word32*)input_data, SHA256_BLOCK_SIZE, BIG_ENDIAN_ORDER);

        input_data += SHA256_BLOCK_SIZE;
        data_len -= SHA256_BLOCK_SIZE;
    }

    // 处理最后一个不满块的数据
    if (data_len > 0) {
        // 将不满块数据复制到缓冲区
        memcpy(ctx.buffer, input_data, data_len);
        ctx.buffer_len = data_len;
    }

    // 记录总比特数
    ctx.num_bits += data_len << 3;

    // 输出最终的SHA-256哈希值
    SHA256_Final(&ctx, hash);

    // 打印SHA-256哈希值
    printf("SHA-256 Hash of \"Hello, world!\": ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    return 0;
}
