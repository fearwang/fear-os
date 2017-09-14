#include <stdio.h>
#include <s3c24xx.h>
#include <syscall.h>

#define TXD0READY   (1<<2)
#define RXD0READY   (1)
int main()
{
    char *tmp = "this is main.elf\n\r";
    int i = 0;
    while(tmp[i]) {
        while (!(UTRSTAT0 & TXD0READY));
        UTXH0 = tmp[i];
        i++;
    }
	
	int test_array[2],ret;
	test_array[0]=0xf0;
	test_array[1]=0x0f;


	SYSCALL(__NR_test,2,test_array,ret);
}

