#include "s3c24xx.h"

#define TXD0READY   (1<<2)
#define RXD0READY   (1)

#define PCLK            50000000    // init.c�е�clock_init��������PCLKΪ50MHz
#define UART_CLK        PCLK        //  UART0��ʱ��Դ��ΪPCLK
#define UART_BAUD_RATE  115200      // ������
#define UART_BRD        ((UART_CLK  / (UART_BAUD_RATE * 16)) - 1)



/*UART registers*/
#define ULCON0_REG_ADDR              (0x50000000)
#define UCON0_REG_ADDR               (0x50000004)
#define UFCON0_REG_ADDR              (0x50000008)
#define UMCON0_REG_ADDR              (0x5000000c)
#define UTRSTAT0_REG_ADDR            (0x50000010)
#define UTXH0_REG_ADDR               (0x50000020)
#define URXH0_REG_ADDR               (0x50000024)
#define UBRDIV0_REG_ADDR             (0x50000028)

/*
 * ��ʼ��UART0
 * 115200,8N1,������
 */
void uart0_init_boot(void)
{
    GPHCON  |= 0xa0;    // GPH2,GPH3����TXD0,RXD0
    GPHUP   = 0x0c;     // GPH2,GPH3�ڲ�����

    ULCON0  = 0x03;     // 8N1(8������λ���޽��飬1��ֹͣλ)
    UCON0   = 0x05;     // ��ѯ��ʽ��UARTʱ��ԴΪPCLK
    UFCON0  = 0x00;     // ��ʹ��FIFO
    UMCON0  = 0x00;     // ��ʹ������
    UBRDIV0 = UART_BRD; // ������Ϊ115200
}

/*
 * ����һ���ַ�
 */
void putc_boot(unsigned char c)
{

    /* �ȴ���ֱ�����ͻ������е������Ѿ�ȫ�����ͳ�ȥ */
    while (!(UTRSTAT0 & TXD0READY));
    

    /* ��UTXH0�Ĵ�����д�����ݣ�UART���Զ��������ͳ�ȥ */
    UTXH0 = c;
}

/*
void putc_mmu_boot(unsigned char c)
{
    volatile unsigned char * utxh0_ptr = (volatile unsigned char *)ioremap(UTXH0_REG_ADDR, 0, 0);
    volatile unsigned long * utrstat0_ptr = (volatile unsigned long *)ioremap(UTRSTAT0_REG_ADDR, 0, 0);

//    while (!(UTRSTAT0 & TXD0READY));
    while( !((*utrstat0_ptr) & TXD0READY) )
           ;
    
    *utxh0_ptr = c;

}*/


/*
 * �����ַ�
 */
unsigned char getc_boot(void)
{
    /* �ȴ���ֱ�����ջ������е������� */
    while (!(UTRSTAT0 & RXD0READY));
    
    /* ֱ�Ӷ�ȡURXH0�Ĵ��������ɻ�ý��յ������� */
    return URXH0;
}


/*
unsigned char getc_mmu(void)
{
    volatile unsigned char * urxh0_ptr = (volatile unsigned char *)ioremap(URXH0_REG_ADDR, 0, 0);
    volatile unsigned long * utrstat0_ptr = (volatile unsigned long *)ioremap(UTRSTAT0_REG_ADDR, 0, 0);
    while( !((*utrstat0_ptr) & RXD0READY) );
    //while (!(UTRSTAT0 & RXD0READY));
    
    return *urxh0_ptr;
}*/

/*
 * �ж�һ���ַ��Ƿ�����
 */
int isDigit_boot(unsigned char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    else
        return 0;       
}

/*
 * �ж�һ���ַ��Ƿ�Ӣ����ĸ
 */
int isLetter_boot(unsigned char c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    else if (c >= 'A' && c <= 'Z')
        return 1;       
    else
        return 0;
}

void puts_boot(char *str)
{
	int i = 0;
	while (str[i])
	{
		putc_boot(str[i]);
		i++;
	}
}

void puthex_boot(unsigned int val)
{
	/* 0x1234abcd */
	int i;
	int j;
	
	puts_boot("0x");

	for (i = 0; i < 8; i++)
	{
		j = (val >> ((7-i)*4)) & 0xf;
		if ((j >= 0) && (j <= 9))
			putc_boot('0' + j);
		else
			putc_boot('A' + j - 0xa);
		
	}
	
}

