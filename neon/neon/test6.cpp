#include <arm_neon.h>
#include <iostream>
#include <iomanip>
#include <cstring>


#define SHA256_ROTL(a,b) (((a>>(32-b))&(0x7fffffff>>(31-b)))|(a<<b))
#define SHA256_SR(a,b) ((a>>b)&(0x7fffffff>>(b-1)))
#define SHA256_Ch(x,y,z) ((x&y)^((~x)&z))
#define SHA256_Maj(x,y,z) ((x&y)^(x&z)^(y&z))
#define SHA256_E0(x) (SHA256_ROTL(x,30)^SHA256_ROTL(x,19)^SHA256_ROTL(x,10))
#define SHA256_E1(x) (SHA256_ROTL(x,26)^SHA256_ROTL(x,21)^SHA256_ROTL(x,7))
#define SHA256_O0(x) (SHA256_ROTL(x,25)^SHA256_ROTL(x,14)^SHA256_SR(x,3))
#define SHA256_O1(x) (SHA256_ROTL(x,15)^SHA256_ROTL(x,13)^SHA256_SR(x,10))

void sha256_neon(const uint8_t* data, size_t len, uint8_t* hash) {
    // Initialize hash values
    uint32_t H[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    // Constants for SHA-256
    const uint32_t K[64] = {
    static_cast<uint32_t>(0x428A2F98), static_cast<uint32_t>(0x71374491), static_cast<uint32_t>(0xB5C0FBCF), static_cast<uint32_t>(0xE9B5DBA5),
    static_cast<uint32_t>(0x3956C25B), static_cast<uint32_t>(0x59F111F1), static_cast<uint32_t>(0x923F82A4), static_cast<uint32_t>(0xAB1C5ED5),
    static_cast<uint32_t>(0xD807AA98), static_cast<uint32_t>(0x12835B01), static_cast<uint32_t>(0x243185BE), static_cast<uint32_t>(0x550C7DC3),
    static_cast<uint32_t>(0x72BE5D74), static_cast<uint32_t>(0x80DEB1FE), static_cast<uint32_t>(0x9BDC06A7), static_cast<uint32_t>(0xC19BF174),
    static_cast<uint32_t>(0xE49B69C1), static_cast<uint32_t>(0xEFBE4786), static_cast<uint32_t>(0x0FC19DC6), static_cast<uint32_t>(0x240CA1CC),
    static_cast<uint32_t>(0x2DE92C6F), static_cast<uint32_t>(0x4A7484AA), static_cast<uint32_t>(0x5CB0A9DC), static_cast<uint32_t>(0x76F988DA),
    static_cast<uint32_t>(0x983E5152), static_cast<uint32_t>(0xA831C66D), static_cast<uint32_t>(0xB00327C8), static_cast<uint32_t>(0xBF597FC7),
    static_cast<uint32_t>(0xC6E00BF3), static_cast<uint32_t>(0xD5A79147), static_cast<uint32_t>(0x06CA6351), static_cast<uint32_t>(0x14292967),
    static_cast<uint32_t>(0x27B70A85), static_cast<uint32_t>(0x2E1B2138), static_cast<uint32_t>(0x4D2C6DFC), static_cast<uint32_t>(0x53380D13),
    static_cast<uint32_t>(0x650A7354), static_cast<uint32_t>(0x766A0ABB), static_cast<uint32_t>(0x81C2C92E), static_cast<uint32_t>(0x92722C85),
    static_cast<uint32_t>(0xA2BFE8A1), static_cast<uint32_t>(0xA81A664B), static_cast<uint32_t>(0xC24B8B70), static_cast<uint32_t>(0xC76C51A3),
    static_cast<uint32_t>(0xD192E819), static_cast<uint32_t>(0xD6990624), static_cast<uint32_t>(0xF40E3585), static_cast<uint32_t>(0x106AA070),
    static_cast<uint32_t>(0x19A4C116), static_cast<uint32_t>(0x1E376C08), static_cast<uint32_t>(0x2748774C), static_cast<uint32_t>(0x34B0BCB5),
    static_cast<uint32_t>(0x391C0CB3), static_cast<uint32_t>(0x4ED8AA4A), static_cast<uint32_t>(0x5B9CCA4F), static_cast<uint32_t>(0x682E6FF3),
    static_cast<uint32_t>(0x748F82EE), static_cast<uint32_t>(0x78A5636F), static_cast<uint32_t>(0x84C87814), static_cast<uint32_t>(0x8CC70208),
    static_cast<uint32_t>(0x90BEFFFA), static_cast<uint32_t>(0xA4506CEB), static_cast<uint32_t>(0xBEF9A3F7), static_cast<uint32_t>(0xC67178F2)
};


    // Padding and length calculation
    uint8_t paddedData[64];
    std::memset(paddedData, 0, sizeof(paddedData));
    std::memcpy(paddedData, data, len);
    paddedData[len] = 0x80;
    uint64_t bitLen = len * 8;
    std::memcpy(paddedData + 56, &bitLen, sizeof(bitLen));

    // Process the data in 64-byte chunks
    for (size_t i = 0; i < len + 8; i += 64) {
        uint32_t W[64];
        uint32x4_t a, b, c, d, e, f, g, h;
        uint32x4_t temp1, temp2, maj, ch;

        // Load the data into W
        for (size_t j = 0; j < 16; j++) {
            W[j] = be32toh(reinterpret_cast<const uint32_t*>(paddedData + i)[j]);
        }
        for (size_t j = 16; j < 64; j++) {
            uint32x4_t s0 = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(vld1q_u32(W + j - 15)), vreinterpretq_u8_u32(vld1q_u32(W + j - 15)), 12));
            uint32x4_t s1 = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(vld1q_u32(W + j - 2)), vreinterpretq_u8_u32(vld1q_u32(W + j - 2)), 12));
            W[j] = vgetq_lane_u32(vaddq_u32(vaddq_u32(vextq_u32(vld1q_u32(W + j - 16), vld1q_u32(W + j - 16), 1), s0), vaddq_u32(vextq_u32(vld1q_u32(W + j - 7), vld1q_u32(W + j - 7), 1), s1)), 0);
        }

        // Load the initial hash values into registers
        a = vld1q_u32(H);
        b = vld1q_u32(H + 4);

        // Main loop
        for (size_t j = 0; j < 64; j++) {
            ch = veorq_u32(vandq_u32(b, c), vbicq_u32(b, d));
            maj = veorq_u32(veorq_u32(vandq_u32(a, b), vandq_u32(a, c)), vandq_u32(b, c));
            uint32x4_t h_vec = vdupq_n_u32(h);
uint32x4_t e_vec = vdupq_n_u32(e);
uint32x4_t ch_vec = vdupq_n_u32(ch);
uint32x4_t w_vec = vdupq_n_u32(W[j]);
temp1 = vaddq_u32(vaddq_u32(h_vec, SHA256_E1(e_vec)), vaddq_u32(vaddq_u32(ch_vec, w_vec), vld1q_u32(K + j)));

            temp2 = vaddq_u32(SHA256_E0(a), maj);
            h = g;
            g = f;
            f = e;
            e = vaddq_u32(d, temp1);
            d = c;
            c = b;
            b = a;
            a = vaddq_u32(temp1, temp2);
        }

        // Update the hash values
        a = vaddq_u32(a, vld1q_u32(H));
        b = vaddq_u32(b, vld1q_u32(H + 4));
        vst1q_u32(H, a);
        vst1q_u32(H + 4, b);
    }

    // Convert the hash values to bytes
    for (size_t i = 0; i < 8; i++) {
        hash[i * 4] = (H[i] >> 24) & 0xFF;
        hash[i * 4 + 1] = (H[i] >> 16) & 0xFF;
        hash[i * 4 + 2] = (H[i] >> 8) & 0xFF;
        hash[i * 4 + 3] = H[i] & 0xFF;
    }
}

int main() {
    const char* input = "Hello, world!";
    uint8_t hash[32];

    sha256_neon(reinterpret_cast<const uint8_t*>(input), std::strlen(input), hash);

    // Print the hash
    std::cout << std::hex << std::setfill('0');
    for (int i = 0; i < 32; i++) {
        std::cout << std::setw(2) << static_cast<int>(hash[i]);
    }
    std::cout << std::endl;

    return 0;
}
