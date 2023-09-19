
#include "riscv_crypto.h"
#include "test_rvkat.h"

//	algorithm tests


int test_sha2();	//	test_sha2.c


//	stub main: run unit tests

int main()
{
	int fail = 0;

	
	
	fail += test_sha2();
	
	//fail=0;

	return 0;
}
