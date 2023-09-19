#include <iostream>
#include <iomanip>
#include <cstring>

#include <arm_neon.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>

using namespace CryptoPP;

typedef uint32_t word32;


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
// SHA-256常量

    // ... 这里放入之前提供的SHA-256常量 ...

// SHA-256初始哈希值

const uint32_t SHA256_H0[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
// SHA-256算法中一个块的大小（以字节为单位）
#define SHA256_BLOCK_SIZE 64

// SHA-256算法中一个块的大小（以字为单位）
#define SHA256_BLOCK_SIZE_WORDS (SHA256_BLOCK_SIZE / sizeof(uint32_t))

// 待哈希数据填充时用到的填充字节
#define SHA256_PADDING_BYTE 0x80

// 右循环移位
#define SHA256_ROTR(x, n) ((x) >> (n) | (x) << (32 - (n)))

// SHA-256上下文结构
typedef struct {
    uint32_t state[8]; // 哈希状态
    uint64_t num_bits; // 总比特数
    uint32_t buffer[SHA256_BLOCK_SIZE_WORDS]; // 数据缓冲区
    size_t buffer_len; // 缓冲区中数据的长度
} SHA256_CTX;




// 初始化SHA-256上下文
void SHA256_Init(SHA256_CTX* ctx) {
    // 将初始哈希值复制到上下文中
    for (int i = 0; i < 8; i++) {
        ctx->state[i] = SHA256_H0[i];
    }

    ctx->num_bits = 0;
    ctx->buffer_len = 0;
}


void SHA256_HashMultipleBlocks_ARMV8(word32 *state, const word32 *data, size_t length, ByteOrder order)
{
    CRYPTOPP_ASSERT(state);
    CRYPTOPP_ASSERT(data);
    CRYPTOPP_ASSERT(length >= SHA256::BLOCKSIZE);

    uint32x4_t STATE0, STATE1, ABEF_SAVE, CDGH_SAVE;
    uint32x4_t MSG0, MSG1, MSG2, MSG3;
    uint32x4_t TMP0, TMP1, TMP2;

    // Load initial values
    STATE0 = vld1q_u32(&state[0]);
    STATE1 = vld1q_u32(&state[4]);

    while (length >= SHA256::BLOCKSIZE)
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

        data += SHA256::BLOCKSIZE/sizeof(word32);
        length -= SHA256::BLOCKSIZE;
    }

    // Save state
    vst1q_u32(&state[0], STATE0);
    vst1q_u32(&state[4], STATE1);
}
// SHA-256填充函数
void SHA256_Pad(SHA256_CTX* ctx) {
    // 添加填充字节
    ctx->buffer[ctx->buffer_len++] = SHA256_PADDING_BYTE;

    // 如果缓冲区已满或剩余的空间不足以容纳消息长度
    if (ctx->buffer_len > SHA256_BLOCK_SIZE_WORDS - 8) {
        while (ctx->buffer_len < SHA256_BLOCK_SIZE_WORDS) {
            ctx->buffer[ctx->buffer_len++] = 0;
        }
        SHA256_HashMultipleBlocks_ARMV8(ctx->state, ctx->buffer, SHA256_BLOCK_SIZE, BIG_ENDIAN_ORDER);
        ctx->buffer_len = 0;
    }

    // 填充0直到缓冲区满
    while (ctx->buffer_len < SHA256_BLOCK_SIZE_WORDS - 8) {
        ctx->buffer[ctx->buffer_len++] = 0;
    }

    // 添加消息长度，以比特为单位
    uint64_t total_bits = ctx->num_bits << 3;
    ctx->buffer[SHA256_BLOCK_SIZE_WORDS - 2] = (total_bits >> 32);
    ctx->buffer[SHA256_BLOCK_SIZE_WORDS - 1] = (total_bits & 0xFFFFFFFF);

    // 处理最后一个块
    SHA256_HashMultipleBlocks_ARMV8(ctx->state, ctx->buffer, SHA256_BLOCK_SIZE, BIG_ENDIAN_ORDER);
}

// 输出SHA-256哈希值
void SHA256_Final(SHA256_CTX* ctx, uint8_t hash[32]) {
    SHA256_Pad(ctx);

    // 将最终的哈希值输出到哈希数组中
    for (int i = 0; i < 8; i++) {
        hash[i * 4] = (ctx->state[i] >> 24) & 0xFF;
        hash[i * 4 + 1] = (ctx->state[i] >> 16) & 0xFF;
        hash[i * 4 + 2] = (ctx->state[i] >> 8) & 0xFF;
        hash[i * 4 + 3] = ctx->state[i] & 0xFF;
    }
}


// 主函数，计算SHA-256哈希值
#include <string.h>

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
