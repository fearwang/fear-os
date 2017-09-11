
/*
 *	#: name                        size            offset          mask_flags
 *	0: bootloader          0x00040000      0x00000000      0
 *	1: params              0x00020000      0x00040000      0
 *	2: kernel              0x00200000      0x00060000      0
 *	3: root                0x0fda0000      0x00260000      0
*/

/* NAND FLASH控制器 */
#define NFCONF (*((volatile unsigned long *)0x4E000000))
#define NFCONT (*((volatile unsigned long *)0x4E000004))
#define NFCMMD (*((volatile unsigned char *)0x4E000008))
#define NFADDR (*((volatile unsigned char *)0x4E00000C))
#define NFDATA (*((volatile unsigned char *)0x4E000010))
#define NFSTAT (*((volatile unsigned char *)0x4E000020))

/* GPIO */
#define GPHCON              (*(volatile unsigned long *)0x56000070)
#define GPHUP               (*(volatile unsigned long *)0x56000078)

/* UART registers*/
#define ULCON0              (*(volatile unsigned long *)0x50000000)
#define UCON0               (*(volatile unsigned long *)0x50000004)
#define UFCON0              (*(volatile unsigned long *)0x50000008)
#define UMCON0              (*(volatile unsigned long *)0x5000000c)
#define UTRSTAT0            (*(volatile unsigned long *)0x50000010)
#define UTXH0               (*(volatile unsigned char *)0x50000020)
#define URXH0               (*(volatile unsigned char *)0x50000024)
#define UBRDIV0             (*(volatile unsigned long *)0x50000028)

#define TXD0READY   (1<<2)


void nand_read_boot(unsigned int addr, unsigned char *buf, unsigned int len);



void nand_init_boot(void)
{
#define TACLS   0
#define TWRPH0  1
#define TWRPH1  0
	/* 设置时序 */
	NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	/* 使能NAND Flash控制器, 初始化ECC, 禁止片选 */
	NFCONT = (1<<4)|(1<<1)|(1<<0);	
}

void nand_select_boot(void)
{
	NFCONT &= ~(1<<1);	
}

void nand_deselect_boot(void)
{
	NFCONT |= (1<<1);	
}

void nand_cmd_boot(unsigned char cmd)
{
	volatile int i;
	NFCMMD = cmd;
	for (i = 0; i < 10; i++);
}

void nand_addr_boot(unsigned int addr)
{
	unsigned int col  = addr % 2048;
	unsigned int page = addr / 2048;
	volatile int i;

	NFADDR = col & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR = (col >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	
	NFADDR  = page & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 16) & 0xff;
	for (i = 0; i < 10; i++);	
}

void nand_wait_ready_boot(void)
{
	while (!(NFSTAT & 1));
}

unsigned char nand_data_boot(void)
{
	return NFDATA;
}

void nand_read_boot(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int col = addr % 2048;
	int i = 0;
	/* 1. 选中 */
	nand_select_boot();

	while (i < len)
	{
		/* 2. 发出读命令00h */
		nand_cmd_boot(0x00);

		/* 3. 发出地址(分5步发出) */
		nand_addr_boot(addr);

		/* 4. 发出读命令30h */
		nand_cmd_boot(0x30);

		/* 5. 判断状态 */
		nand_wait_ready_boot();

		/* 6. 读数据 */
		for (; (col < 2048) && (i < len); col++)
		{
			buf[i] = nand_data_boot();
			i++;
			addr++;
		}
		
		col = 0;
	}
	/* 7. 取消选中 */		
	nand_deselect_boot();
}
