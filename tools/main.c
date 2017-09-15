#include <stdio.h>
#include <s3c24xx.h>
#include <syscall.h>
#include <malloc.h>

#define TXD0READY   (1<<2)
#define RXD0READY   (1)
char *tmp = "this is main.elf\n\r";
char *ret_str = "[main.elf] ret = ";
char *malloc_str = "xfter syscall_malloc addr = ";
void main()
{

	
    int i = 0;
	/*
    while(tmp[i]) {
        while (!(UTRSTAT0 & TXD0READY));
        UTXH0 = tmp[i];
        i++;
    }
	
	int test_array[2],ret;
	test_array[0]=0xf0;
	test_array[1]=0x0f;


	SYSCALL(__NR_test, 2, test_array, ret);
	
	i = 0;
    while(ret_str[i]) {
        while (!(UTRSTAT0 & TXD0READY));
        UTXH0 = ret_str[i];
        i++;
    }
	while (!(UTRSTAT0 & TXD0READY));
    UTXH0 = ret+0x30;
	
	while (!(UTRSTAT0 & TXD0READY));
    UTXH0 = '\n';
        
	while (!(UTRSTAT0 & TXD0READY));
    UTXH0 = '\r';
	
	
	*/
	//------------------------------------
	
	int test_array2[2],ret2;
	test_array2[0] = 64;
	test_array2[1] = 64;
	
	SYSCALL(__NR_malloc, 2, test_array2, ret2);
	
	i = 0;
    while(malloc_str[i]) {
        while (!(UTRSTAT0 & TXD0READY));
        UTXH0 = malloc_str[i];
        i++;
    }
	
	while (!(UTRSTAT0 & TXD0READY));
    UTXH0 = ret2+0x30;
	
	while (!(UTRSTAT0 & TXD0READY));
    UTXH0 = '\n';
        
	while (!(UTRSTAT0 & TXD0READY));
    UTXH0 = '\r';
	

 
}


