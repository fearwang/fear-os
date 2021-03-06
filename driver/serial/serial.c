#include <s3c24xx.h>
#include <serial.h>
#include <ioremap.h>

#define TXD0READY   (1<<2)
#define RXD0READY   (1)

#define PCLK            50000000    // init.c中的clock_init函数设置PCLK为50MHz
#define UART_CLK        PCLK        //  UART0的时钟源设为PCLK
#define UART_BAUD_RATE  115200      // 波特率
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
 * 初始化UART0
 * 115200,8N1,无流控
 */
void uart0_init(void)
{
    GPHCON  |= 0xa0;    // GPH2,GPH3用作TXD0,RXD0
    GPHUP   = 0x0c;     // GPH2,GPH3内部上拉

    ULCON0  = 0x03;     // 8N1(8个数据位，无较验，1个停止位)
    UCON0   = 0x05;     // 查询方式，UART时钟源为PCLK
    UFCON0  = 0x00;     // 不使用FIFO
    UMCON0  = 0x00;     // 不使用流控
    UBRDIV0 = UART_BRD; // 波特率为115200
}

/*
 * 发送一个字符
 */
void putc(unsigned char c)
{

    /* 等待，直到发送缓冲区中的数据已经全部发送出去 */
    while (!(UTRSTAT0 & TXD0READY));
    

    /* 向UTXH0寄存器中写入数据，UART即自动将它发送出去 */
    UTXH0 = c;
}


void putc_mmu(unsigned char c)
{
    volatile unsigned char * utxh0_ptr = (volatile unsigned char *)ioremap(UTXH0_REG_ADDR, 0, 0);
    volatile unsigned long * utrstat0_ptr = (volatile unsigned long *)ioremap(UTRSTAT0_REG_ADDR, 0, 0);

    /* 等待，直到发送缓冲区中的数据已经全部发送出去 */
//    while (!(UTRSTAT0 & TXD0READY));
    while( !((*utrstat0_ptr) & TXD0READY) )
           ;
    
    *utxh0_ptr = c;

    /* 向UTXH0寄存器中写入数据，UART即自动将它发送出去 */
}



/*
 * 接收字符
 */
unsigned char getc(void)
{
    /* 等待，直到接收缓冲区中的有数据 */
    while (!(UTRSTAT0 & RXD0READY));
    
    /* 直接读取URXH0寄存器，即可获得接收到的数据 */
    return URXH0;
}



unsigned char getc_mmu(void)
{
    volatile unsigned char * urxh0_ptr = (volatile unsigned char *)ioremap(URXH0_REG_ADDR, 0, 0);
    volatile unsigned long * utrstat0_ptr = (volatile unsigned long *)ioremap(UTRSTAT0_REG_ADDR, 0, 0);
    /* 等待，直到接收缓冲区中的有数据 */
    while( !((*utrstat0_ptr) & RXD0READY) );
    //while (!(UTRSTAT0 & RXD0READY));
    
    /* 直接读取URXH0寄存器，即可获得接收到的数据 */
    return *urxh0_ptr;
}

/*
 * 判断一个字符是否数字
 */
int isDigit(unsigned char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    else
        return 0;       
}

/*
 * 判断一个字符是否英文字母
 */
int isLetter(unsigned char c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    else if (c >= 'A' && c <= 'Z')
        return 1;       
    else
        return 0;
}

void puts(char *str)
{
	int i = 0;
	while (str[i])
	{
		putc(str[i]);
		i++;
	}
}

void puthex(unsigned int val)
{
	/* 0x1234abcd */
	int i;
	int j;
	
	puts("0x");

	for (i = 0; i < 8; i++)
	{
		j = (val >> ((7-i)*4)) & 0xf;
		if ((j >= 0) && (j <= 9))
			putc('0' + j);
		else
			putc('A' + j - 0xa);
		
	}
	
}

void put_1()
{
	
	putc('\r');
	putc('\n');
	putc('1');
}

void put_2()
{
	
	putc('\r');
	putc('\n');
	putc('2');
}

void put_3()
{
	
	putc('\r');
	putc('\n');
	putc('3');
}

void put_4()
{
	
	putc('\r');
	putc('\n');
	putc('4');
}

void put_5()
{
	
	putc('\r');
	putc('\n');
	putc('5');
}

void put_6()
{
	
	putc('\r');
	putc('\n');
	putc('6');
}

void put_7()
{
	
	putc('\r');
	putc('\n');
	putc('7');
}
