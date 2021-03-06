#include <serial.h>
#include <string.h>
#include <stdio.h>
#include <s3c24xx.h>
#include <debug.h>
#include <mem_layout.h>
#include <driver_core.h>
#include <kmalloc.h>
#include <fs.h>
#include <elf.h>
#include <proc.h>


char *hint  = "fear@jz2440$ ";
char *label  = "Fear OS V1.0";
extern void enable_irq(void);
extern void disable_irq(void);
extern void umask_int(unsigned int offset);
extern void nand_read(unsigned int addr, unsigned char *buf, unsigned int len);
extern void kthread_daemon1(void* args);
extern int romfs_init();

void timer_init(void){

	TCFG0|=0xFA00;		//prescaler
	TCFG1|=0x30000;		//divider
	TCNTB4=120;  		//timer cnt , cnt-- = 0, timer inerrupt ocurr  12500/1s
	
	TCON&=(~(7<<20));	//clear bit 20-22 
	TCON|=(1<<22);		//set bit 22
	TCON|=(1<<21);		//set bit 21   update 	TCNTB4 to  

	

	TCON|=(1<<20);		//start timer 4
	TCON&=~(1<<21);		//clear bit 21, when we at autoload mode

	umask_int(14);		//umask timer4 interrupt
	enable_irq();
}

void print_mem_layout(void)
{
	pr_debug("[TOTAL_PHY_MEM   ] START = %x, END = %x\n",  PLAT_MEM_START, PLAT_MEM_START+PLAT_MEM_SIZE-1);
	pr_debug("[PLAT_HEAP_MEM   ] START = %x, END = %x\n", PLAT_HEAP_MEM_START, PLAT_HEAP_MEM_END-1);
	pr_debug("[RAM_DISK_MEM    ] START = %x, END = %x\n", RAM_DISK_START, RAM_DISK_START+RAM_DISK_SIZE-1);
	pr_debug("[PAGE_TABLE_MEM  ] START = %x, END = %x\n", PAGE_TABLE_BASE_ADDR, PAGE_TABLE_BASE_ADDR+PAGE_TABLE_SIZE-1);
	pr_debug("[SYSTEM_STACK_MEM] START = %x, END = %x\n", SYS_STACK-SYS_STACK_SIZE, PLAT_MEM_START+PLAT_MEM_SIZE-1);
}

void format_ramdisk()
{
	printk("format_ramdisk, copy from nand(%x) to dram(%x)\n", 0x260000, storage[RAMDISK]->start_pos);
	nand_read(0x260000, storage[RAMDISK]->start_pos, 0x200000);
	printk("format_ramdisk done\n");
}


void delay(void){
	volatile unsigned int time=0xffff;
	while(time--);
}

int  test_process(void *p){
	while(1){
		delay();
		pr_info("test user process cpsr = %x, sp = %x\n",get_cpsr(), get_sp());
	}
	return 0;
}

int  test_process2(void *p){
	while(1){
		delay();
		pr_info("test user process 2 cpsr = %x, sp = %x\n",get_cpsr(), get_sp());
	}
	return 0;
}

void swapper_process()
{
	while(1){
		unsigned int sp = get_sp();
		unsigned int cpsr = get_cpsr();
		static int cnt = 0;
		
		pr_info("swapper process %dth, sp=%x, cpsr=%x\n",cnt++,sp,cpsr);
		delay();
	}
}

int start_kernel()
{
    unsigned char c;
	
    int prev_new_line = 0;
    uart0_init();   // 波特率115200，8N1(8个数据位，无校验位，1个停止位)
	disable_irq();
    printk("using printk:%s\n", label);

    init_sys_mmu();
    start_mmu();
    
	irq_init();
	
    int i = 0;
	timer_init();
	print_mem_layout();
	driver_init();
	romfs_init();
	init_page_map();
	kmalloc_init();
	//format_ramdisk();
	
	//-------------------------------test buddy-------------------------------
	/*
	char *p1,*p2,*p3,*p4;
	p1=(char *)get_free_pages(0,7);
	pr_debug("the return address of get_free_pages %x\n",p1);
	p2=(char *)get_free_pages(0,7);
	pr_debug("the return address of get_free_pages %x\n",p2);
	put_free_pages(p2,7);
	put_free_pages(p1,7);
	
	p3=(char *)get_free_pages(0,6);
	pr_debug("the return address of get_free_pages %x\n",p3);
	p4=(char *)get_free_pages(0,6);
	pr_debug("the return address of get_free_pages %x\n",p4);
	put_free_pages(p4,6);
	put_free_pages(p3,6);
	
	
	p1=kmalloc(127);
	pr_debug("the first alloced address is %x\n",p1);
	p2=kmalloc(124);
	pr_debug("the second alloced address is %x\n",p2);
	kfree(p1);
	kfree(p2);
	p3=kmalloc(119);
	pr_debug("the third alloced address is %x\n",p3);
	p4=kmalloc(512);
	pr_debug("the forth alloced address is %x\n",p4);
	*/
	
	//-----------------------------------test romfs-------------------------------
	/*
	char buf[1024];
	//storage[RAMDISK]->read(storage[RAMDISK],buf,0,sizeof(buf));
	//hexdump(buf, 1024);
	struct inode *node;
	node=fs_type[ROMFS]->namei(fs_type[ROMFS],"init.rc");
	fs_type[ROMFS]->device->read(fs_type[ROMFS]->device,buf,fs_type[ROMFS]->get_daddr(node),node->dsize);
	pr_debug("read from %s:\n", node->name);
	for(i = 0;i < node->dsize; i++){
		printk("%c",buf[i]);
	}
	kfree(node);
	*/
		
	//-------------------------------test elf-----------------------------------
	//struct inode *node;
	/*
	struct elf32_phdr *phdr;
	struct elf32_ehdr *ehdr;
	int pos,dpos;
	char *buf2;
	if((buf2 = kmalloc(1024)) == (void *)0) {
		printk("get free pages error\n");
		goto HALT;
	}

	if((node = fs_type[ROMFS]->namei(fs_type[ROMFS],"main.elf")) == (void *)0){
		printk("inode read eror\n");
		goto HALT;
	}

	if( fs_type[ROMFS]->device->read(fs_type[ROMFS]->device,buf2,fs_type[ROMFS]->get_daddr(node),1024) ) {
		printk("read error\n");
		goto HALT;
	}

	ehdr = (struct elf32_ehdr *)buf2;
	phdr = (struct elf32_phdr *)((char *)buf2 + ehdr->e_phoff);

	for(i = 0;i < ehdr->e_phnum; i++) {
		if(CHECK_PT_TYPE_LOAD(phdr)) {
			printk("read elf_header, va=%x, len=%x\n",(char *)phdr->p_vaddr, phdr->p_filesz);
			if(fs_type[ROMFS]->device->read(fs_type[ROMFS]->device, (char *)phdr->p_vaddr, fs_type[ROMFS]->get_daddr(node)+phdr->p_offset,phdr->p_filesz)<0) {
				printk("dout error\n");
				goto HALT;
			}
			phdr++;
		}
	}
	*/
	
	
	//-----------------------------------process--------------------------------------
	do_fork(test_process,(void *)0x1);
	kthread_create(kthread_daemon1, (void*)0x01);
	do_fork(test_process2,(void *)0x2);
	
	//swapper_process();

	printk("%s", hint);
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
	
HALT:
	while(1);
}
