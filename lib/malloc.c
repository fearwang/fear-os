#include <stdio.h>
#include <s3c24xx.h>
#include <syscall.h>

void* malloc(unsigned int size)
{
	unsigned int test_array[1], ret;
	test_array[0] = size;

	SYSCALL(__NR_malloc, 1, test_array, ret);
	
	printk("after syscall_malloc addr = %x\n", (unsigned int)ret);
	
	return (void*)ret;
}