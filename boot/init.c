/*
 * init.c: 进行一些初始化
 */ 

#include "s3c24xx.h"
#include "serial.h"
 
void disable_watch_dog(void);
void clock_init(void);
void memsetup(void);
void copy_steppingstone_to_sdram(void);

/*
 * 关闭WATCHDOG，否则CPU会不断重启
 */
void disable_watch_dog(void)
{
    WTCON = 0;  // 关闭WATCHDOG很简单，往这个寄存器写0即可
}

#define S3C2440_MPLL_200MHZ     ((0x5c<<12)|(0x01<<4)|(0x02))
/*
 * 对于MPLLCON寄存器，[19:12]为MDIV，[9:4]为PDIV，[1:0]为SDIV
 * 有如下计算公式：
 *  S3C2410: MPLL(FCLK) = (m * Fin)/(p * 2^s)
 *  S3C2440: MPLL(FCLK) = (2 * m * Fin)/(p * 2^s)
 *  其中: m = MDIV + 8, p = PDIV + 2, s = SDIV
 * 对于本开发板，Fin = 12MHz
 * 设置CLKDIVN，令分频比为：FCLK:HCLK:PCLK=1:2:4，
 * FCLK=200MHz,HCLK=100MHz,PCLK=50MHz
 */
void clock_init(void)
{
    // LOCKTIME = 0x00ffffff;   // 使用默认值即可
    CLKDIVN  = 0x03;            // FCLK:HCLK:PCLK=1:2:4, HDIVN=1,PDIVN=1

    /* 如果HDIVN非0，CPU的总线模式应该从“fast bus mode”变为“asynchronous bus mode” */
__asm__(
    "mrc    p15, 0, r1, c1, c0, 0\n"        /* 读出控制寄存器 */ 
    "orr    r1, r1, #0xc0000000\n"          /* 设置为“asynchronous bus mode” */
    "mcr    p15, 0, r1, c1, c0, 0\n"        /* 写入控制寄存器 */
    );

    MPLLCON = S3C2440_MPLL_200MHZ;  /* 现在，FCLK=200MHz,HCLK=100MHz,PCLK=50MHz */
}

/*
 * 设置存储控制器以使用SDRAM
 */
void memsetup(void)
{
    volatile unsigned long *p = (volatile unsigned long *)MEM_CTL_BASE;

    /* 这个函数之所以这样赋值，而不是像前面的实验(比如mmu实验)那样将配置值
     * 写在数组中，是因为要生成”位置无关的代码”，使得这个函数可以在被复制到
     * SDRAM之前就可以在steppingstone中运行
     */
    /* 存储控制器13个寄存器的值 */
    p[0] = 0x22011110;     //BWSCON
    p[1] = 0x00000700;     //BANKCON0
    p[2] = 0x00000700;     //BANKCON1
    p[3] = 0x00000700;     //BANKCON2
    p[4] = 0x00000700;     //BANKCON3  
    p[5] = 0x00000700;     //BANKCON4
    p[6] = 0x00000700;     //BANKCON5
    p[7] = 0x00018005;     //BANKCON6
    p[8] = 0x00018005;     //BANKCON7
    
                                            /* REFRESH,
                                             * HCLK=12MHz:  0x008C07A3,
                                             * HCLK=100MHz: 0x008C04F4
                                             */ 
    p[9]  = 0x008C04F4;
    p[10] = 0x000000B1;     //BANKSIZE
    p[11] = 0x00000030;     //MRSRB6
    p[12] = 0x00000030;     //MRSRB7
}

void copy_steppingstone_to_sdram(void)
{
    unsigned char *pdwSrc  = (unsigned char *)0;
    unsigned char *pdwDest = (unsigned char *)0x30000000;
    
    while (pdwSrc < (unsigned char *)4096)
    {
        *pdwDest = *pdwSrc;
        pdwDest++;
        pdwSrc++;
    }
}

void print_relocate_length(unsigned int len)
{
    puts_boot("\n\rrelocate length=");
    puthex_boot(len);
    puts_boot("\n\r");
}


//extern __ro_start__;
//extern __rw_end__;
//extern __bss_start__;

void relocate_img_to_dram(unsigned char *src, unsigned char *dst, unsigned int len)
{
    //BUG FIX : loacl string also store with link addr, so before we jump to the link addr , we must not use strimgs 
//    print_relocate_length(len);
    nand_read_boot(src, dst, len);

}


void copy_kernel_from_nand(unsigned char *src, unsigned char *dst, unsigned int len)
{
    //unsigned char *dst = (unsigned char*)0x30000000;
    //unsigned long  src_flash= 0x0;
    //int size = __rw_end__ - __ro_start__;
    //int size = 8192;
    print_relocate_length(len);
    nand_read_boot(src, dst, len);

}

/*
void puts_hello()
{
    puts("hello\r\n");

}*/

void compare_dram_and_sram_4k()
{
    unsigned  short i = 0;
    unsigned char *src = (unsigned char *)0;
    unsigned char *dst = (unsigned char *)0x30000000;

    for(i = 0; i < 4096; i++) {
        if(*src != *dst)
            break;
        src++;
        dst++;
    }

    int b1, b2, b3, b4;
    b1 = (i&0xf000) >> 12;
    if(b1 > 9)
        b1 += 0x41-10;
    else
        b1 += 0x30;
    b2 = (i&0x0f00) >> 8;
    if(b2 > 9)
        b2 += 0x41-10;
    else
        b2 += 0x30;
    b3 = (i&0x00f0) >> 4;
    if(b3 > 9)
        b3 += 0x41-10;
    else
        b3 += 0x30;
    b4 = (i&0x000f) >> 0;
    if(b4 > 9)
        b4 += 0x41-10;
    else
        b4 += 0x30;
    puts_boot("\r\ni=0x");
    putc_boot(b1);
    putc_boot(b2);
    putc_boot(b3);
    putc_boot(b4);
    puts_boot("->");


    if(i == 4096) {
        puts_boot("rel ok\r\n");
    } else {
        puts_boot("rel fail\r\n");
    }
}


