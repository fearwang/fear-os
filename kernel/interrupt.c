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
	unsigned int tmp=(1<<INTOFFSET);
	printk("%d\t",INTOFFSET);
	SRCPND|=tmp;
	INTPND|=tmp;
	//enable_irq();
	printk("interrupt occured\n\r");
	//disable_irq();
}


