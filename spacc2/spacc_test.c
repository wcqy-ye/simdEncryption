#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "spacc.h"

#define DEBUG 1
unsigned long start,end,alltime=0;
static char src[1024] = {0}, dst[1024] = {0};

static void exec_and_dump_result(int fd, unsigned char *src, unsigned char *result, int len)
{
	int i, ret;

	ret = write(fd, src, len);
	if (ret < 0) {
		printf("write failed\n");
		return;
	}

	ret = read(fd, result, len);
	if (ret < 0) {
		printf("read failed\n");
		return;
	}

#if DEBUG
	for (i = 0; i < len; i++)
	{
		printf("%x ", result[i]);
	}

	printf("\n");
#endif	
}

// set and get config
int test_case_1(void)
{
	struct spacc_config config, get_config = {0};
	int fd = open("/dev/spacc", O_RDWR);
	int i;

	memset(config.key, 0x61, 16);
	memset(config.iv, 0x61, 16);
	config.algo = SPACC_ALGO_AES;
	config.mode = SPACC_ALGO_MODE_CBC;
	config.key_size = SPACC_KEY_SIZE_128BITS;
	config.action = SPACC_ACTION_ENCRYPTION;

	if (ioctl(fd, IOCTL_SPACC_SET_CONFIG, &config) == 0)
	{
		printf(" ok\n");
	}

	if (ioctl(fd, IOCTL_SPACC_GET_CONFIG, &get_config) == 0)
	{
		printf(" ok\n");
	}

	printf("key: %s\n", get_config.key);
	printf("iv: %s\n", get_config.iv);
	printf("algo: %d\n", get_config.algo);
	printf("mode: %d\n", get_config.mode);
	printf("key_size: %d\n", get_config.key_size);
	printf("action: %d\n", get_config.action);

	close(fd);
	return 0;
}

#define uint8_t unsigned char

unsigned long read_time(void)
{
    unsigned long cycles;
    asm volatile ("rdtime %0" : "=r" (cycles));
    return cycles;
}

int test_case_sha256(int fd,uint8_t *src,size_t src_len,uint8_t hash[32])
{
        //printf("src:%s\n",src);
        //printf("src_len:%d\n",src_len);
	struct spacc_config config = {0};
	int i, ret;
	int BLOCK_SIZE=64;
	
	
	//size_t padding_len = BLOCK_SIZE - (src_len % BLOCK_SIZE);
	//size_t total_len = src_len + padding_len;
	size_t total_len =  (src_len + 9 + 63) & ~63;
	uint8_t padded_src[total_len];

	memcpy(padded_src, src, src_len);
	padded_src[src_len] = 0x80;
	for (size_t i = src_len + 1; i < total_len - 8; i++) {
        padded_src[i] = 0;
    }
	uint64_t bits_len_be = src_len * 8;
    for (int i = 0; i < 8; i++) {
        padded_src[total_len - 8 + i] = (uint8_t)(bits_len_be >> (56 - i * 8));
    }
	config.algo = SPACC_ALGO_SHA256;
	if (ioctl(fd, IOCTL_SPACC_SET_CONFIG, &config)) {
		printf("ioctl failed\n");
		return -1;
	}

	//unsigned long start, end;
	start = read_time();
	ret = write(fd, padded_src, total_len);


	if (ret < 0) {
		printf("write failed\n");
		return -1;
	}

	ret = read(fd, dst, 32);
	if (ret < 0) {
		printf("read failed\n");
		return -1;
	}	

	end = read_time();
	//printf("Took %lu us\n", (end - start)/25);
        alltime+=end - start;
	
        memcpy(hash, dst, sizeof(uint8_t) * 32);
	return 0;
}

int main(int argc, char* argv[])
{
        if (argc != 2)   
        return 1;
        uint8_t hash[32];
	uint32_t size = 512;
	int fd = open("/dev/spacc", O_RDWR);

	if (fd < 0) {
		printf("open /dev/spacc failed\n");
		return -1;
	}

	// set buffer size
	if (ioctl(fd, IOCTL_SPACC_SET_BUFFER_SIZE, &size)) {
		printf("ioctl failed\n");
		return -1;
	}

	size = 0;
	if (ioctl(fd, IOCTL_SPACC_GET_BUFFER_SIZE, &size)) {
		printf("ioctl failed\n");
		return -1;
	}
	
        
        
        
	printf("buffer size : %d\n", size);
	printf("testcase sha256\n");
	for(int i=0;i<10;i++)
	test_case_sha256(fd,(uint8_t *)argv[1],strlen(argv[1]),hash);
        printf("10 times Took %lu us\n", alltime/25);
        
        for (int i = 0; i < 32; i++)
	{
		printf("%02x ", hash[i]);
	}

	printf("\n");
	close(fd);

	return 0;
}

