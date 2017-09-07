#include <serial.h>
#include <string.h>
#include <stdio.h>
#include <s3c24xx.h>

char *hint  = "fear@jz2440$ ";
char *label  = "\n\rFear OS !!!\n\r";
extern void enable_irq(void);
extern void umask_int(unsigned int offset);

void timer_init(void){

	TCFG0|=0xFE00;
	TCFG1|=0x1000;
	TCON&=(~(7<<20));
	TCON|=(1<<22);
	TCON|=(1<<21);

	TCNTB4=50000;

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
	
	//timer_init();
	
	init_page_map();
	kmalloc_init();
	
	char *p1,*p2,*p3,*p4;
	p1=(char *)get_free_pages(0,6);
	printk("the return address of get_free_pages %x\n",p1);
	p2=(char *)get_free_pages(0,6);
	printk("the return address of get_free_pages %x\n",p2);
	put_free_pages(p2,6);
	put_free_pages(p1,6);
	p3=(char *)get_free_pages(0,7);
	printk("the return address of get_free_pages %x\n",p3);
	p4=(char *)get_free_pages(0,7);
	printk("the return address of get_free_pages %x\n",p4);
	put_free_pages(p4,7);
	put_free_pages(p3,7);
	
	p1=kmalloc(127);
	printk("the first alloced address is %x\n",p1);
	p2=kmalloc(124);
	printk("the second alloced address is %x\n",p2);
	kfree(p1);
	kfree(p2);
	p3=kmalloc(119);
	printk("the third alloced address is %x\n",p3);
	p4=kmalloc(512);
	printk("the forth alloced address is %x\n",p4);
	
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
