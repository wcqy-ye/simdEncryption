#include <stdio.h>
#include <arm_neon.h>
#include <sys/time.h>
float sum_array(float *arr, int len)
{
    if(NULL == arr || len < 1)
    {
       printf("input error\n");
        return 0;
    }
    float sum;
    int i;
    for(i=0; i<len; ++i)
    {
        sum += *arr++;
    }
    return sum;
}
float sum_array_neon(float *arr, int len)
{
    if(NULL == arr || len < 1)
    {
       printf("input error\n");
        return 0;
    }
 
    int dim4 = len >> 2; // 数组长度除4整数
    int left4 = len & 3; // 数组长度除4余数
    float32x4_t sum_vec = vdupq_n_f32(0.0);//定义用于暂存累加结果的寄存器且初始化为0
    for (; dim4>0; dim4--, arr+=4) //每次同时访问4个数组元素
    {
        float32x4_t data_vec = vld1q_f32(arr); //依次取4个元素存入寄存器vec
        sum_vec = vaddq_f32(sum_vec, data_vec);//ri = ai + bi 计算两组寄存器对应元素之和并存放到相应结果
    }
    float sum = vgetq_lane_f32(sum_vec, 0)+vgetq_lane_f32(sum_vec, 1)+vgetq_lane_f32(sum_vec, 2)+vgetq_lane_f32(sum_vec, 3);//将累加结果寄存器中的所有元素相加得到最终累加值
    for (; left4>0; left4--, arr++)
        sum += (*arr) ;   //对于剩下的少于4的数字，依次计算累加即可
    return sum;
}
#define size 10240000
float test[size];
int main()
{
     struct timeval start,end;
     int i;
     float sum_result;
     for(i=0;i<size;i++)
        test[i] = 1;
     gettimeofday(&start,NULL);
     sum_result = sum_array_neon(test,size);
     gettimeofday(&end,NULL);  
     printf("neon use time %dus\n",1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec));
 
     gettimeofday(&start,NULL);
     sum_result = sum_array(test,size);
     gettimeofday(&end,NULL);  
     printf("no neon use time %dus\n",1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec));
	return 0;
	
}
