#include <s3c24xx.h>

void enable_irq(void){
	asm volatile (
		"mrs r4,cpsr\n\t"
		"bic r4,r4,#0x80\n\t"
		"msr cpsr,r4\n\t"
		:::"r4"
	);
}

void disable_irq(void){
	asm volatile (
		"mrs r4,cpsr\n\t"
		"orr r4,r4,#0x80\n\t"
		"msr cpsr,r4\n\t"
		:::"r4"
	);
}

void umask_int(unsigned int offset){
	INTMSK&=~(1<<offset);
}

void common_irq_handler(void){
	unsigned int tmp = (1<<INTOFFSET);
	//printk("%d\t",INTOFFSET);
	SRCPND|=tmp;
	INTPND|=tmp;
	enable_irq();
	/* 即使发生了中断 在中断向量中 汇编不会破坏现场，而且当调用c代码时，编译器会保存函数使用的寄存器，因此
	 * 中断嵌套发生的时候，前一个执行环境并不会被破坏
	 */
	//printk("interrupt occured\n\r"); 
	disable_irq();
}


