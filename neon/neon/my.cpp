#include <iostream>
#include <arm_neon.h>
#include <cstdint>
#include <cstring>
#include <cstdio>

using namespace std;

uint32x4_t exchange(uint32x4_t MSG,const uint8x16_t MASK)
{
uint8x8_t lowPart,highPart,lowShuffled,highShuffled;
    uint8x16_t shuffled;
    
    lowPart = vget_low_u8(vreinterpretq_u8_u32(MSG));
         highPart = vget_high_u8(vreinterpretq_u8_u32(MSG));
         lowShuffled = vtbl1_u8(lowPart, vget_low_u8(MASK));
         highShuffled = vtbl1_u8(highPart, vget_high_u8(MASK));
         shuffled = vcombine_u8(lowShuffled, highShuffled);
         return vreinterpretq_u32_u8(shuffled);
       
}



uint32x4_t sha256rnds2(uint32x4_t a, uint32x4_t b, uint32x4_t msg)
{
    uint32x4_t ab_xor = veorq_u32(a, b);
    uint32x4_t bc_and = vandq_u32(b, msg);
    uint32x4_t bd_bic = vbicq_u32(b, msg);
    uint32x4_t bc_xor_bd = veorq_u32(bc_and, bd_bic);
    uint32x4_t result = veorq_u32(ab_xor, bc_xor_bd);
    return result;
}
uint32x4_t sha256msg1(uint32x4_t a, uint32x4_t b) {
    uint32x4_t result;

    // Extract the elements from 'a' and 'b'
    uint32_t a3 = vgetq_lane_u32(a, 3);
    uint32_t a2 = vgetq_lane_u32(a, 2);
    uint32_t b1 = vgetq_lane_u32(b, 1);
    uint32_t b0 = vgetq_lane_u32(b, 0);

    // Perform the addition
    uint32_t sum03 = a3 + b0;
    uint32_t sum12 = a2 + b1;

    // Combine the results
    result = vsetq_lane_u32(sum03, result, 3);
    result = vsetq_lane_u32(sum12, result, 2);
    result = vsetq_lane_u32(a2, result, 1);
    result = vsetq_lane_u32(a3, result, 0);

    return result;
}


uint32x4_t sha256msg2(uint32x4_t a, uint32x4_t b) {
    uint32x4_t result;

    // Extract the elements from 'a' and 'b'
    uint32_t a1 = vgetq_lane_u32(a, 1);
    uint32_t a0 = vgetq_lane_u32(a, 0);
    uint32_t b3 = vgetq_lane_u32(b, 3);
    uint32_t b2 = vgetq_lane_u32(b, 2);

    // Perform the addition
    uint32_t sum01 = a0 + b2;
    uint32_t sum23 = a1 + b3;

    // Combine the results
    result = vsetq_lane_u32(sum01, result, 0);
    result = vsetq_lane_u32(sum23, result, 1);
    result = vsetq_lane_u32(a1, result, 2);
    result = vsetq_lane_u32(a0, result, 3);

    return result;
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

void blockNeonSha(uint32_t state[8], const uint8_t data[], uint32_t length)
{
    uint32x4_t STATE0, STATE1;
    uint32x4_t MSG, TMP;
    uint32x4_t MSG0, MSG1, MSG2, MSG3;
    uint32x4_t ABEF_SAVE, CDGH_SAVE;
    //const uint8x16_t MASK = vcreate_u8(0x0c0d0e0f08090a0bULL, 0x0405060700010203ULL);
const uint8x16_t MASK = vreinterpretq_u8_u64(vcombine_u64(vcreate_u64(0x0405060700010203ULL), vcreate_u64(0x0c0d0e0f08090a0bULL)));

    /* Load initial values */
    TMP = vld1q_u32(state);
    STATE1 = vld1q_u32(state + 4);
    uint8x16_t maskt = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};
    TMP = shuffle_u32(TMP, 0xB1);         /* CDAB */
    STATE1 = shuffle_u32(STATE1, 0x1B);      /* EFGH */
    STATE0 = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(TMP), vreinterpretq_u8_u32(STATE1), 8));         /* ABEF */
    STATE1 = vreinterpretq_u32_u8(vbslq_u8(maskt, vreinterpretq_u8_u32(STATE1), vreinterpretq_u8_u32(TMP)));         /* CDGH */
 
    while (length >= 64)
    {
        /* Save current state */
        ABEF_SAVE = STATE0;
        CDGH_SAVE = STATE1;

        /* Rounds 0-3 */
        MSG = vreinterpretq_u32_u8(vld1q_u8(data));
        MSG0=exchange(MSG,MASK);
        //MSG0 = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(MSG), MASK));
        MSG = vaddq_u32(MSG0, vcombine_u32(vcreate_u32(0xE9B5DBA5B5C0FBCFULL), vcreate_u32(0x71374491428A2F98ULL)));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        MSG = shuffle_u32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);

        /* Rounds 4-7 */
        MSG1 = vreinterpretq_u32_u8(vld1q_u8(data + 16));

         MSG1=exchange(MSG1,MASK);
        //MSG1 = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(MSG1), MASK));
        MSG = vaddq_u32(MSG1, vcombine_u32(vcreate_u32(0xAB1C5ED5923F82A4ULL), vcreate_u32(0x59F111F13956C25BULL)));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        MSG = shuffle_u32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);
        MSG0 = sha256msg1(MSG0, MSG1);

        /* Rounds 8-11 */
        MSG2 = vreinterpretq_u32_u8(vld1q_u8(data + 32));
         MSG2=exchange(MSG2,MASK);
        //MSG2 = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(MSG2), MASK));
        MSG = vaddq_u32(MSG2, vcombine_u32(vcreate_u32(0x550C7DC3243185BEULL), vcreate_u32(0x12835B01D807AA98ULL)));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        MSG = shuffle_u32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);
        MSG1 = sha256msg1(MSG1, MSG2);

        /* Rounds 12-15 */
        MSG3 = vreinterpretq_u32_u8(vld1q_u8(data + 48));
         MSG3=exchange(MSG3,MASK);
        //MSG3 = vreinterpretq_u32_u8(vqtbl1q_u8(vreinterpretq_u8_u32(MSG3), MASK));
        MSG = vaddq_u32(MSG3, vcombine_u32(vcreate_u32(0xC19BF1749BDC06A7ULL), vcreate_u32(0x80DEB1FE72BE5D74ULL)));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        //TMP = vextq_u32(MSG3, MSG2, 1);//msg3的第一个和msg2后三个元素返回
        //vextq_u8(TMP, STATE1, 8);
        TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG3), vreinterpretq_u8_u32(MSG2), 4));

        MSG0 = vaddq_u32(MSG0, TMP);
        MSG0 = sha256msg2(MSG0, MSG3);
        MSG = shuffle_u32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);
        MSG2 = sha256msg1(MSG2, MSG3);

        /* Rounds 16-19 */
        MSG = vaddq_u32(MSG0, vcombine_u32(vcreate_u32(0xC19BF1749BDC06A7ULL), vcreate_u32(0x80DEB1FE72BE5D74ULL)));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG0), vreinterpretq_u8_u32(MSG3), 4));//TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG1 = vaddq_u32(MSG1, TMP);//MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG1 = sha256msg2(MSG1, MSG0);//MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = shuffle_u32(MSG, 0x0E);//MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);//STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG3 = sha256msg1(MSG3, MSG0);//MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

         /* Rounds 20-23 */
        MSG = vaddq_u32(MSG0, vcombine_u32(vcreate_u32(0x76F988DA5CB0A9DCULL), vcreate_u32(0x4A7484AA2DE92C6FULL)));
//MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG0), vreinterpretq_u8_u32(MSG3), 4));//TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG2 = vaddq_u32(MSG2, TMP);//MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG2 = sha256msg2(MSG2, MSG1);//MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = shuffle_u32(MSG, 0x0E);//MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);//STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG0 = sha256msg1(MSG0, MSG1);//MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

/* Rounds 24-27 */
        MSG = vaddq_u32(MSG2, vcombine_u32(vcreate_u32(0xBF597FC7B00327C8ULL), vcreate_u32(0xA831C66D983E5152ULL)));
//MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG2), vreinterpretq_u8_u32(MSG1), 4));//TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG3 = vaddq_u32(MSG3, TMP);//MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG3 = sha256msg2(MSG3, MSG2);//MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = shuffle_u32(MSG, 0x0E);//MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);//STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG1 = sha256msg1(MSG1, MSG2);//MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

/* Rounds 28-31 */
        MSG = vaddq_u32(MSG3, vcombine_u32(vcreate_u32(0x1429296706CA6351ULL), vcreate_u32(0xD5A79147C6E00BF3ULL)));
//MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG3), vreinterpretq_u8_u32(MSG2), 4));//TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG0 = vaddq_u32(MSG0, TMP);//MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG0 = sha256msg2(MSG0, MSG3);//MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = shuffle_u32(MSG, 0x0E);//MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);//STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG2 = sha256msg1(MSG2, MSG3);//MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

/* Rounds 32-35 */
        MSG = vaddq_u32(MSG0, vcombine_u32(vcreate_u32(0x53380D134D2C6DFCULL), vcreate_u32(0x2E1B213827B70A85ULL)));
//MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG0), vreinterpretq_u8_u32(MSG3), 4));//TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG1 = vaddq_u32(MSG1, TMP);//MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG1 = sha256msg2(MSG1, MSG0);//MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = shuffle_u32(MSG, 0x0E);//MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);//STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG3 = sha256msg1(MSG3, MSG0);//MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

        /* Rounds 36-39 */
        MSG = vaddq_u32(MSG1, vcombine_u32(vcreate_u32(0x92722C8581C2C92EULL), vcreate_u32(0x766A0ABB650A7354ULL)));
//MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG1), vreinterpretq_u8_u32(MSG0), 4));//TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG2 = vaddq_u32(MSG2, TMP);//MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG2 = sha256msg2(MSG2, MSG1);//MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = shuffle_u32(MSG, 0x0E);//MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);//STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG0 = sha256msg1(MSG0, MSG1);//MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

    /* Rounds 40-43 */
        MSG = vaddq_u32(MSG2, vcombine_u32(vcreate_u32(0xC76C51A3C24B8B70ULL), vcreate_u32(0xA81A664BA2BFE8A1ULL)));
//MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG2), vreinterpretq_u8_u32(MSG1), 4));//TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG3 = vaddq_u32(MSG3, TMP);//MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG3 = sha256msg2(MSG3, MSG2);//MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = shuffle_u32(MSG, 0x0E);//MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);//STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG1 = sha256msg1(MSG1, MSG2);//MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

    /* Rounds 44-47 */
        MSG = vaddq_u32(MSG3, vcombine_u32(vcreate_u32(0x106AA070F40E3585ULL), vcreate_u32(0xD6990624D192E819ULL)));
//MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG3), vreinterpretq_u8_u32(MSG2), 4));//TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG0 = vaddq_u32(MSG0, TMP);//MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG0 = sha256msg2(MSG0, MSG3);//MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = shuffle_u32(MSG, 0x0E);//MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);//STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG2 = sha256msg1(MSG2, MSG3);//MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0); 

    /* Rounds 48-51 */
        MSG = vaddq_u32(MSG0, vcombine_u32(vcreate_u32(0x34B0BCB52748774CULL), vcreate_u32(0x1E376C0819A4C116ULL)));
//MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG0), vreinterpretq_u8_u32(MSG3), 4));//TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG1 = vaddq_u32(MSG1, TMP);//MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG1 = sha256msg2(MSG1, MSG0);//MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = shuffle_u32(MSG, 0x0E);//MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);//STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG3 = sha256msg1(MSG3, MSG0);//MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

        /* Rounds 52-55 */
        MSG = vaddq_u32(MSG1, vcombine_u32(vcreate_u32(0x682E6FF35B9CCA4FULL), vcreate_u32(0x4ED8AA4A391C0CB3ULL)));
//MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG1), vreinterpretq_u8_u32(MSG0), 4));//TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG2 = vaddq_u32(MSG2, TMP);//MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG2 = sha256msg2(MSG2, MSG1);//MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = shuffle_u32(MSG, 0x0E);//MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);//STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        //MSG3 = sha256msg1(MSG3, MSG0);//MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);


    /* Rounds 56-59 */
        MSG = vaddq_u32(MSG2, vcombine_u32(vcreate_u32(0x8CC7020884C87814ULL), vcreate_u32(0x78A5636F748F82EEULL)));
//MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG2), vreinterpretq_u8_u32(MSG1), 4));//TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG3 = vaddq_u32(MSG3, TMP);//MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG3 = sha256msg2(MSG3, MSG2);//MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = shuffle_u32(MSG, 0x0E);//MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);//STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        //MSG3 = sha256msg1(MSG3, MSG0);//MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

    /* Rounds 60-63 */
        MSG = vaddq_u32(MSG3, vcombine_u32(vcreate_u32(0xC67178F2BEF9A3F7ULL), vcreate_u32(0xA4506CEB90BEFFFAULL)));
//MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
        STATE1 = sha256rnds2(STATE1, STATE0, MSG);
        //TMP = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(MSG0), vreinterpretq_u8_u32(MSG3), 4));//TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
       // MSG1 = vaddq_u32(MSG1, TMP);//MSG1 = _mm_add_epi32(MSG1, TMP);
        //MSG1 = sha256msg2(MSG1, MSG0);//MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = shuffle_u32(MSG, 0x0E);//MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = sha256rnds2(STATE0, STATE1, MSG);//STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        //MSG3 = sha256msg1(MSG3, MSG0);//MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);


        /* Combine state */
        STATE0 = vaddq_u32(STATE0, ABEF_SAVE);
        STATE1 = vaddq_u32(STATE1, CDGH_SAVE);

        data += 64;
        length -= 64;
    }

    //TMP = vextq_u32(STATE1, STATE0, 3);        /* FEBA */
    //STATE1 = vextq_u32(STATE0, STATE1, 1);     /* DCHG */
    //STATE0 = vextq_u32(TMP, STATE0, 1);        /* DCBA */
    //STATE1 = vextq_u32(STATE1, TMP, 3);        /* ABEF */

    TMP = shuffle_u32(STATE0, 0x1B);         /* FEBA */
    STATE1 = shuffle_u32(STATE1, 0xB1);      /* DCHG */
    STATE0 = vreinterpretq_u32_u8(vbslq_u8(maskt, vreinterpretq_u8_u32(TMP),vreinterpretq_u8_u32(STATE1) ));    /* DCBA */     
    STATE1 = vreinterpretq_u32_u8(vextq_u8(vreinterpretq_u8_u32(STATE1),vreinterpretq_u8_u32(TMP), 8));         /* ABEF */

    /* Save state */
    vst1q_u32(state, STATE0);
    vst1q_u32(state + 4, STATE1);
}
int main() {
    /* empty message with padding */
    uint8_t message[64];
    memset(message, 0x00, sizeof(message));
    message[0] = 0x80;

    /* initial state */
    uint32_t state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    blockNeonSha(state, message, sizeof(message)); // size in bytes

    const uint8_t b1 = (uint8_t)(state[0] >> 24);
    const uint8_t b2 = (uint8_t)(state[0] >> 16);
    const uint8_t b3 = (uint8_t)(state[0] >>  8);
    const uint8_t b4 = (uint8_t)(state[0] >>  0);
    const uint8_t b5 = (uint8_t)(state[1] >> 24);
    const uint8_t b6 = (uint8_t)(state[1] >> 16);
    const uint8_t b7 = (uint8_t)(state[1] >>  8);
    const uint8_t b8 = (uint8_t)(state[1] >>  0);

    printf("SHA256 hash of empty message: %d\n", sizeof(message));

    /* e3b0c44298fc1c14... */
    printf("SHA256 hash of empty message: ");
    printf("%02X%02X%02X%02X%02X%02X%02X%02X...\n",
        b1, b2, b3, b4, b5, b6, b7, b8);

    int success = ((b1 == 0xE3) && (b2 == 0xB0) && (b3 == 0xC4) && (b4 == 0x42) &&
                    (b5 == 0x98) && (b6 == 0xFC) && (b7 == 0x1C) && (b8 == 0x14));

    if (success)
        printf("Success!\n");
    else
        printf("Failure!\n");

    return (success != 0 ? 0 : 1);
}






