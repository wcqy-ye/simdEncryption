#include <iostream>
#include <arm_neon.h>
#include <cstdint>
#include <cstring>
#include <cstdio>

using namespace std;

uint8_t* StrSHA256(const uint8_t* str, long long length, char* sha256)
{
	uint8_t *pp, *ppend;
    long l, i, W[64], T1, T2, A, B, C, D, E, F, G, H, H0, H1, H2, H3, H4, H5, H6, H7;
    uint32_t h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
	// Constants for SHA-256
    const uint32_t kt[64] = {
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
    l = length + ((length % 64 > 56) ? (128 - length % 64) : (64 - length % 64));
	if (!(pp = (uint8_t*)malloc((unsigned long)l))) return 0;
	for (i = 0; i < length; pp[i + 3 - 2 * (i % 4)] = str[i], i++);
	for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l; pp[i + 3 - 2 * (i % 4)] = 0, i++);
	*((long*)(pp + l - 4)) = length << 3;
    *((long*)(pp + l - 8)) = length >> 29;
    //sprintf(sha256, "%08X%08X%08X%08X%08X%08X%08X%08X", H0, H1, H2, H3, H4, H5, H6, H7);
    //puts(sha256);
    STATE0 = vld1q_u32(&state[0]);
    STATE1 = vld1q_u32(&state[4]);
    while (l >= 64)
    {
		//uint32_t W[64];
		uint32x4_t w,k,wk;
        // 保存当前状态
        h1234_SAVE = STATE0;
        h5678_SAVE = STATE1;
        
        
        // Load the data into W
        for (size_t j = 0; j < 4; j++) {
			w=vld1q_u32((const uint32_t*) (pp+0));
            k=vld1q_u32( (kt+j*4));
            wk=vaddq_u32(k);
            
            
            
        }
        for (size_t j = 16; j < 64; j++) {
            uint32x4_t s0 = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(vld1q_u32(W + j - 15)), vreinterpretq_u8_u32(vld1q_u32(W + j - 15)), 12));
            uint32x4_t s1 = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(vld1q_u32(W + j - 2)), vreinterpretq_u8_u32(vld1q_u32(W + j - 2)), 12));
            W[j] = vgetq_lane_u32(vaddq_u32(vaddq_u32(vextq_u32(vld1q_u32(W + j - 16), vld1q_u32(W + j - 16), 1), s0), vaddq_u32(vextq_u32(vld1q_u32(W + j - 7), vld1q_u32(W + j - 7), 1), s1)), 0);
        }

        // Rounds 0-3
        // 处理输入数据的前16字节
        MSG = vld1q_u32((const uint32_t*) (pp+0));
        //MSG0 = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(MSG), vreinterpretq_u8_u64(vld1q_u64((const uint64_t*) &MASK))));
        MSG = vaddq_u32(MSG, vld1q_u32((const uint32_t*) {0xE9B5DBA5, 0xB5C0FBCF, 0x71374491, 0x428A2F98}));
        
        
        //STATE1 = vsha256h2q_u32(STATE1, STATE0, MSG);
        MSG = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(MSG), vreinterpretq_u8_u32(vdupq_n_u32(0x0E0F0809))));
        STATE0 = vsha256h2q_u32(STATE0, STATE1, MSG);

        // Rounds 4-7
        // 处理输入数据的16-31字节
        MSG1 = vld1q_u32((const uint32_t*) (data+16));
        MSG1 = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(MSG1), vreinterpretq_u8_u64(vld1q_u64((const uint64_t*) &MASK))));
        MSG = vaddq_u32(MSG1, vld1q_u32((const uint32_t*) {0xAB1C5ED5, 0x923F82A4, 0x59F111F1, 0x3956C25B}));
        STATE1 = vsha256h2q_u32(STATE1, STATE0, MSG);
        MSG = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(MSG), vreinterpretq_u8_u32(vdupq_n_u32(0x0E0F0809))));
        STATE0 = vsha256h2q_u32(STATE0, STATE1, MSG);
        MSG0 = vsha256su1q_u32(MSG0, MSG1);

        // Rounds 8-11
        // 处理输入数据的32-47字节
        MSG2 = vld1q_u32((const uint32_t*) (data+32));
        MSG2 = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(MSG2), vreinterpretq_u8_u64(vld1q_u64((const uint64_t*) &MASK))));
        MSG = vaddq_u32(MSG2, vld1q_u32((const uint32_t*) {0x550C7DC3, 0x243185BE, 0x12835B01, 0xD807AA98}));
        STATE1 = vsha256h2q_u32(STATE1, STATE0, MSG);
        MSG = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(MSG), vreinterpretq_u8_u32(vdupq_n_u32(0x0E0F0809))));
        STATE0 = vsha256h2q_u32(STATE0, STATE1, MSG);
        MSG1 = vsha256su1q_u32(MSG1, MSG2);

        // Rounds 12-15
        // 处理输入数据的48-63字节
        MSG3 = vld1q_u32((const uint32_t*) (data+48));
        MSG3 = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(MSG3), vreinterpretq_u8_u64(vld1q_u64((const uint64_t*) &MASK))));
        MSG = vaddq_u32(MSG3, vld1q_u32((const uint32_t*) {0xC19BF174, 0x9BDC06A7, 0x80DEB1FE, 0x72BE5D74}));
        STATE1 = vsha256h2q_u32(STATE1, STATE0, MSG);
        TMP = vextq_u32(MSG3, MSG2, 1);
        MSG0 = vaddq_u32(MSG0, TMP);
        MSG0 = vsha256su0q_u32(MSG0, MSG3);
        MSG = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(MSG), vreinterpretq_u8_u32(vdupq_n_u32(0x0E0F0809))));
        STATE0 = vsha256h2q_u32(STATE0, STATE1, MSG);
        MSG2 = vsha256su1q_u32(MSG2, MSG3);

        // 合并状态
        STATE0 = vaddq_u32(STATE0, ABEF_SAVE);
        STATE1 = vaddq_u32(STATE1, CDGH_SAVE);

        data += 64;
        l -= 64;
    }
}

int main()
{
	const char* input = "abc";
	char sha256[256];
	StrSHA256(reinterpret_cast<const uint8_t*>(input),3,sha256);  
	
	return 0;
	}
