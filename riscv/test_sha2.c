

#include "riscv_crypto.h"
#include "test_rvkat.h"
#include "sha2_api.h"



int test_sha2_256_tv()
{
	//	Padding tests

	uint8_t md[32], d[256];
	int fail = 0;
	int i;

	sha2_256(md, "abc", 3);

	printf("sha256 md:");			   
for (int i = 0; i < 32; i++) {
        printf("%02x", md[i]); 
    }
	printf("\n");
	//	SHA2-224
	
	
	return fail;
}

//	SHA2-384/512 test vectors


//	SHA2: algorithm tests

int test_sha2()
{
	int fail = 0;

	rvkat_info("=== SHA2-256 using sha2_cf256_rvk() ===");
	sha256_compress = sha2_cf256_rvk;
	fail += test_sha2_256_tv();


	return fail;
}

