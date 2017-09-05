#include <serial.h>
#include <string.h>
#include <stdio.h>
#include <s3c24xx.h>

char *hint  = "fear@jz2440$ ";
char *label  = "\n\rFear OS !!!\n\r";
extern void enable_irq(void);
extern void umask_int(unsigned int offset);

void timer_init(void){

	TCFG0|=0x800;
	TCON&=(~(7<<20));
	TCON|=(1<<22);
	TCON|=(1<<21);

	TCNTB4=1000000;

	TCON|=(1<<20);
	TCON&=~(1<<21);

	umask_int(14);
	enable_irq();
}

int start_kernel()
{
    unsigned char c;
    int prev_new_line = 0;
    uart0_init();   // 波特率115200，8N1(8个数据位，无校验位，1个停止位)

    puts(label);
    printk("using printk:%s", label);

    init_sys_mmu();
    start_mmu();
    
    int i = 0;
    for(i = 0; i < strlen(hint); i++) {
        putc_mmu(*(hint+i));
    }
	
	timer_init();
	
    while(1)
    {
        // 从串口接收数据后，判断其是否数字或子母，若是则加1后输出
        c = getc_mmu();
        if (isDigit(c) || isLetter(c)) {
            if(prev_new_line == 1)
                prev_new_line = 0;
            putc_mmu(c);
        }
        if(c == '\n') {
            prev_new_line = 1;
            putc_mmu('n');
        } 
        if(c == '\r') {
            prev_new_line = 0;
            putc_mmu('\n');
            putc_mmu('\r');
            for(i = 0; i < strlen(hint); i++) {
                putc_mmu(*(hint+i));
            }
        }
    }

    return 0;
}
