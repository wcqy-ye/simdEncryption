
//SHA-256
/*理解算法最重要，最好自己动手实现试试看，可以使用MFC写一个简单的交互界面*/

#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

#define SHA256_ROTL(a,b) (((a>>(32-b))&(0x7fffffff>>(31-b)))|(a<<b))
#define SHA256_SR(a,b) ((a>>b)&(0x7fffffff>>(b-1)))
#define SHA256_Ch(x,y,z) ((x&y)^((~x)&z))
#define SHA256_Maj(x,y,z) ((x&y)^(x&z)^(y&z))
#define SHA256_E0(x) (SHA256_ROTL(x,30)^SHA256_ROTL(x,19)^SHA256_ROTL(x,10))
#define SHA256_E1(x) (SHA256_ROTL(x,26)^SHA256_ROTL(x,21)^SHA256_ROTL(x,7))
#define SHA256_O0(x) (SHA256_ROTL(x,25)^SHA256_ROTL(x,14)^SHA256_SR(x,3))
#define SHA256_O1(x) (SHA256_ROTL(x,15)^SHA256_ROTL(x,13)^SHA256_SR(x,10))
char* StrSHA256(const char* str, long long length, char* sha256){
    char *pp, *ppend;
    long l, i, W[64], T1, T2, A, B, C, D, E, F, G, H, H0, H1, H2, H3, H4, H5, H6, H7;
    H0 = 0x6a09e667, H1 = 0xbb67ae85, H2 = 0x3c6ef372, H3 = 0xa54ff53a;
    H4 = 0x510e527f, H5 = 0x9b05688c, H6 = 0x1f83d9ab, H7 = 0x5be0cd19;
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
    l = length + ((length % 64 > 56) ? (128 - length % 64) : (64 - length % 64));
    if (!(pp = (char*)malloc((unsigned long)l))) return 0;
    for (i = 0; i < length; pp[i + 3 - 2 * (i % 4)] = str[i], i++);
    for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l; pp[i + 3 - 2 * (i % 4)] = 0, i++);
    *((long*)(pp + l - 4)) = length << 3;
    *((long*)(pp + l - 8)) = length >> 29;
    for (ppend = pp + l; pp < ppend; pp += 64){
        for (i = 0; i < 16; W[i] = ((long*)pp)[i], i++);
        for (i = 16; i < 64; W[i] = (SHA256_O1(W[i - 2]) + W[i - 7] + SHA256_O0(W[i - 15]) + W[i - 16]), i++);
        A = H0, B = H1, C = H2, D = H3, E = H4, F = H5, G = H6, H = H7;
        for (i = 0; i < 64; i++){
            T1 = H + SHA256_E1(E) + SHA256_Ch(E, F, G) + K[i] + W[i];
            T2 = SHA256_E0(A) + SHA256_Maj(A, B, C);
            H = G, G = F, F = E, E = D + T1, D = C, C = B, B = A, A = T1 + T2;
        }
        H0 += A, H1 += B, H2 += C, H3 += D, H4 += E, H5 += F, H6 += G, H7 += H;
    }
    free(pp - l);
    sprintf(sha256, "%08X%08X%08X%08X%08X%08X%08X%08X", H0, H1, H2, H3, H4, H5, H6, H7);
    return sha256;
}
char* FileSHA256(const char* file, char* sha256){

    FILE* fh;
    char* addlp, T[64];
    long addlsize, j, W[64], T1, T2, A, B, C, D, E, F, G, H, H0, H1, H2, H3, H4, H5, H6, H7;
    long long length, i, cpys;
    void *pp, *ppend;
    H0 = 0x6a09e667, H1 = 0xbb67ae85, H2 = 0x3c6ef372, H3 = 0xa54ff53a;
    H4 = 0x510e527f, H5 = 0x9b05688c, H6 = 0x1f83d9ab, H7 = 0x5be0cd19;
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
    fh = fopen(file, "rb");
    fseek(fh, 0, SEEK_END);
    length = ftello64(fh);
    addlsize = (56 - length % 64 > 0) ? (64) : (128);
    if (!(addlp = (char*)malloc(addlsize))) return 0;
    cpys = ((length - (56 - length % 64)) > 0) ? (length - length % 64) : (0);
    j = (long)(length - cpys);
    if (!(pp = (char*)malloc(j))) return 0;
    fseek(fh, -j, SEEK_END);
    fread(pp, 1, j, fh);
    for (i = 0; i < j; addlp[i + 3 - 2 * (i % 4)] = ((char*)pp)[i], i++);
    free(pp);
    for (addlp[i + 3 - 2 * (i % 4)] = 128, i++; i < addlsize; addlp[i + 3 - 2 * (i % 4)] = 0, i++);
    *((long*)(addlp + addlsize - 4)) = length << 3;
    *((long*)(addlp + addlsize - 8)) = length >> 29;
    for (rewind(fh); 64 == fread(W, 1, 64, fh);){
        for (i = 0; i < 64; T[i + 3 - 2 * (i % 4)] = ((char*)W)[i], i++);
        for (i = 0; i < 16; W[i] = ((long*)T)[i], i++);
        for (i = 16; i < 64; W[i] = (SHA256_O1(W[i - 2]) + W[i - 7] + SHA256_O0(W[i - 15]) + W[i - 16]), i++);
        A = H0, B = H1, C = H2, D = H3, E = H4, F = H5, G = H6, H = H7;
        for (i = 0; i < 64; i++){
            T1 = H + SHA256_E1(E) + SHA256_Ch(E, F, G) + K[i] + W[i];
            T2 = SHA256_E0(A) + SHA256_Maj(A, B, C);
            H = G, G = F, F = E, E = D + T1, D = C, C = B, B = A, A = T1 + T2;
        }
        H0 += A, H1 += B, H2 += C, H3 += D, H4 += E, H5 += F, H6 += G, H7 += H;
    }
    for (pp = addlp, ppend = addlp + addlsize; pp < ppend; pp = (long*)pp + 16){
        for (i = 0; i < 16; W[i] = ((long*)pp)[i], i++);
        for (i = 16; i < 64; W[i] = (SHA256_O1(W[i - 2]) + W[i - 7] + SHA256_O0(W[i - 15]) + W[i - 16]), i++);
        A = H0, B = H1, C = H2, D = H3, E = H4, F = H5, G = H6, H = H7;
        for (i = 0; i < 64; i++){
            T1 = H + SHA256_E1(E) + SHA256_Ch(E, F, G) + K[i] + W[i];
            T2 = SHA256_E0(A) + SHA256_Maj(A, B, C);
            H = G, G = F, F = E, E = D + T1, D = C, C = B, B = A, A = T1 + T2;
        }
        H0 += A, H1 += B, H2 += C, H3 += D, H4 += E, H5 += F, H6 += G, H7 += H;
    }
    free(addlp); fclose(fh);
    sprintf(sha256, "%08X%08X%08X%08X%08X%08X%08X%08X", H0, H1, H2, H3, H4, H5, H6, H7);
    return sha256;
}

char* StrSHA256(const char* str, long long length, char* sha256);

int main(void){
    char text[256];
    
    cout<<"请输入原文：\n" ;
    while(cin>>text)
    {
        cout<<"请输入原文：\n" ;
        char sha256[256];
        StrSHA256(text,sizeof(text)-1,sha256);  // sizeof()包含了末尾的终止符'\0'故 -1
        cout<<"执行SHA-256算法后的结果如下：\n";
        puts(sha256);

    }

    system("pause");
    return 0;
}
