// 实验二：
// 写一个简单的程序来验证SIMD在效率方面的提升
#include <iostream>
#include <nmmintrin.h>
#include <immintrin.h>
#include <string>
#include <time.h>
#include <stdlib.h>

void print_time_result(std::string func_name, clock_t start, clock_t finish, int res)
{
    double time = (double(finish - start) / double(CLOCKS_PER_SEC)) * 1000; // ms
    std::cout << func_name << ": " << time << "ms, result = " << res << std::endl;
}

// 普通的加法
void normal_add(int *nums, size_t n)
{
    clock_t start = clock();
    int sum = 0;
    for (size_t i = 0; i < n; i++)
    {
        sum += nums[i];
    }
    clock_t finish = clock();
    print_time_result("normal_add", start, finish, sum);
}

// 使用128bit的SIMD
void SSE_add(int *nums, size_t n)
{
    clock_t start = clock();

    __m128i simd_sum = _mm_setzero_si128();// 累加和
    int normal_sum = 0;

    size_t loop = n / 4;
    size_t reserve = n % 4;

    __m128i *p = (__m128i *)nums;
    for (size_t i = 0; i < loop; i++)
    {
        simd_sum = _mm_add_epi32(simd_sum, *p);
        p++;
    }

    int *q = (int *)p;
    for (size_t i = 0; i < reserve; i++)
    {
        normal_sum += q[i];
    }
    normal_sum += (((int *)&simd_sum)[0] + ((int *)&simd_sum)[1] + ((int *)&simd_sum)[2] + ((int *)&simd_sum)[3]);

    clock_t finish = clock();
    print_time_result("SSE_add", start, finish, normal_sum);
}

// 使用256bit的SIMD
void AVX2_add(int *nums, size_t n)
{
    clock_t start = clock();

    __m256i simd_sum = _mm256_setzero_si256();
    __m256i simd_load;
    int normal_sum = 0;

    size_t loop = n / 8;
    size_t reserve = n % 8;

    __m256i *p = (__m256i *)nums;
    for (size_t i = 0; i < loop; i++)
    {
        simd_load = _mm256_load_si256((const __m256i *)p);
        simd_sum = _mm256_add_epi32(simd_sum, simd_load);
        p++;
    }

    int *q = (int *)p;
    for (size_t i = 0; i < reserve; i++)
    {
        normal_sum += q[i];
    }
    normal_sum += (((int *)&simd_sum)[0] + ((int *)&simd_sum)[1] +
                   ((int *)&simd_sum)[2] + ((int *)&simd_sum)[3] +
                   ((int *)&simd_sum)[4] + ((int *)&simd_sum)[5] +
                   ((int *)&simd_sum)[6] + ((int *)&simd_sum)[7]);

    clock_t finish = clock();
    print_time_result("AVX2_add", start, finish, normal_sum);
}

int main()
{
    size_t n = 10000000;
    int *nums = NULL;
    // nums = (int *)malloc(sizeof(int) * n);
    posix_memalign((void **)&nums, 32, sizeof(int) * n); // 注意要32字节对齐，否则会段错误

    for (size_t i = 0; i < n; i++)
    {
        nums[i] = 5;
    }
    normal_add(nums, n);
    SSE_add(nums, n);
    AVX2_add(nums, n);
    return 0;
}
