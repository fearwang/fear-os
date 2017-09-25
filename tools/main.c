#include <stdio.h>
#include <s3c24xx.h>
#include <syscall.h>
#include <malloc.h>
#include <proc.h>

#define TXD0READY   (1<<2)
#define RXD0READY   (1)
char *tmp = "this is main.elf\n\r";
char *loop = "this is main.elf loop cnt = ";
char *ret_str = "[main.elf] ret = ";
char *malloc_str = "xfter syscall_malloc addr = ";



void putc_elf(unsigned char c)
{

    /* 等待，直到发送缓冲区中的数据已经全部发送出去 */
    while (!(UTRSTAT0 & TXD0READY));
    

    /* 向UTXH0寄存器中写入数据，UART即自动将它发送出去 */
    UTXH0 = c;
}

void puts_elf(char *str)
{
	int i = 0;
	while (str[i])
	{
		putc_elf(str[i]);
		i++;
	}
}

void puthex_elf(unsigned int val)
{
	/* 0x1234abcd */
	int i;
	int j;
	
	puts_elf("0x");

	for (i = 0; i < 8; i++)
	{
		j = (val >> ((7-i)*4)) & 0xf;
		if ((j >= 0) && (j <= 9))
			putc_elf('0' + j);
		else
			putc_elf('A' + j - 0xa);
		
	}
	
}

void delay(void)
{
	volatile unsigned int time = 229320;
	while(time--)
		;
}

void main()
{

	unsigned int sp = get_sp();
	puts_elf("sp_in main, ");
	puthex_elf(sp);
	puts_elf("\n\r");
    
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
	/*
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
	*/
	
	//--------------------------------------------------
	unsigned int cpsr = get_cpsr();
	//cpsr = cpsr & 0x1f;
	puts_elf("mode bit = ");
	puthex_elf(cpsr);
	puts_elf("\n\r");
	
	unsigned int cnt = 0;
	while(1) {
		
		unsigned int sp = get_sp();
		puts_elf("sp_in while(1), ");
		puthex_elf(sp);
		puts_elf(" : ");
		
		unsigned int cpsr = get_cpsr();
		//cpsr = cpsr & 0x1f;
		puts_elf("mode bit while(1), ");
		puthex_elf(cpsr);
		puts_elf(" : ");
		
		puts_elf(loop);
		puthex_elf(cnt);
		puts_elf("\n\r");
		cnt++;
		delay();
	}

 
 
}




