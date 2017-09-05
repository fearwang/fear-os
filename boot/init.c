/*
 * init.c: ����һЩ��ʼ��
 */ 

#include "s3c24xx.h"
#include "serial.h"
 
void disable_watch_dog(void);
void clock_init(void);
void memsetup(void);
void copy_steppingstone_to_sdram(void);

/*
 * �ر�WATCHDOG������CPU�᲻������
 */
void disable_watch_dog(void)
{
    WTCON = 0;  // �ر�WATCHDOG�ܼ򵥣�������Ĵ���д0����
}

#define S3C2440_MPLL_200MHZ     ((0x5c<<12)|(0x01<<4)|(0x02))
/*
 * ����MPLLCON�Ĵ�����[19:12]ΪMDIV��[9:4]ΪPDIV��[1:0]ΪSDIV
 * �����¼��㹫ʽ��
 *  S3C2410: MPLL(FCLK) = (m * Fin)/(p * 2^s)
 *  S3C2440: MPLL(FCLK) = (2 * m * Fin)/(p * 2^s)
 *  ����: m = MDIV + 8, p = PDIV + 2, s = SDIV
 * ���ڱ������壬Fin = 12MHz
 * ����CLKDIVN�����Ƶ��Ϊ��FCLK:HCLK:PCLK=1:2:4��
 * FCLK=200MHz,HCLK=100MHz,PCLK=50MHz
 */
void clock_init(void)
{
    // LOCKTIME = 0x00ffffff;   // ʹ��Ĭ��ֵ����
    CLKDIVN  = 0x03;            // FCLK:HCLK:PCLK=1:2:4, HDIVN=1,PDIVN=1

    /* ���HDIVN��0��CPU������ģʽӦ�ôӡ�fast bus mode����Ϊ��asynchronous bus mode�� */
__asm__(
    "mrc    p15, 0, r1, c1, c0, 0\n"        /* �������ƼĴ��� */ 
    "orr    r1, r1, #0xc0000000\n"          /* ����Ϊ��asynchronous bus mode�� */
    "mcr    p15, 0, r1, c1, c0, 0\n"        /* д����ƼĴ��� */
    );

    MPLLCON = S3C2440_MPLL_200MHZ;  /* ���ڣ�FCLK=200MHz,HCLK=100MHz,PCLK=50MHz */
}

/*
 * ���ô洢��������ʹ��SDRAM
 */
void memsetup(void)
{
    volatile unsigned long *p = (volatile unsigned long *)MEM_CTL_BASE;

    /* �������֮����������ֵ����������ǰ���ʵ��(����mmuʵ��)����������ֵ
     * д�������У�����ΪҪ���ɡ�λ���޹صĴ��롱��ʹ��������������ڱ����Ƶ�
     * SDRAM֮ǰ�Ϳ�����steppingstone������
     */
    /* �洢������13���Ĵ�����ֵ */
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


