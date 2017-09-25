
/*

	sub r14,r14,#4			//计算中断发生时的状态的返回地址(可能处于user或svc模式)
	str r14,[r13,#-0x4]		//并将其保存
	mrs r14,spsr			//spsr_irq保存到r14_irq中
	str r14,[r13,#-0x8]		//压栈，即将上一个状态的cpsr保存起来 即使再发生中断 lr和cpsr都不会被破坏
	str r0,[r13,#-0xc]		//在irq模式的堆栈中保存r0
	mov r0,r13				//使用r0保存r13_irq的值给sys模式  sys模式从中取得lr_irq 和spsr_irq
	CHANGE_TO_SYS		
	str r14,[r13,#-0x8]  	//同样是保存r14_sys
	ldr r14,[r0,#-0x4]		//设置r14_svc为r0传递过来的值 即r13_irq
	str r14,[r13,#-0x4]		//从r13_irq回复出上一个状态的返回地址

    sub r13,r13,#0x8

	ldr r14,[r0,#-0x8]
	ldr r0,[r0,#-0xc]
	stmdb r13!,{r0-r3,r14}
	bl common_irq_handler
	ldmia r13!,{r0-r3,r14}
	msr spsr,r14
	ldmfd r13!,{r14,pc}^
	
	


*/

#include <s3c24xx.h>

#define NULL (void *)0
#define TIMER4_IRQ_NUM 14

typedef int (*irq_handler)(void *arg);

irq_handler  irq_handler_table[64];

void debug_tmp()
{
	printk("after irq handler\n");
}

int register_irq(irq_handler handler, int irq_num, int flag)
{
	if(irq_handler_table[irq_num] != NULL) {
		printk("irq %d already register before.\n");
		return -1;
	}
		
	irq_handler_table[irq_num] = handler;
	return 0;
}

int timer4_irq_handler(void *arg)
{
	static int timer4_irq_cnt = 0;
	printk("timer4_irq_handler exec, %d\n", timer4_irq_cnt++);
}

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
	unsigned int irq_num = INTOFFSET;
	unsigned int tmp = (1 << INTOFFSET);
	//printk("%d\t",INTOFFSET);
	SRCPND |= tmp;
	INTPND |= tmp;
	enable_irq();
	/* 即使发生了中断 在中断向量中 汇编不会破坏现场，而且当调用c代码时，编译器会保存函数使用的寄存器，因此
	 * 中断嵌套发生的时候，前一个执行环境并不会被破坏
	 */
	//printk("interrupt occured\n\r");
	irq_handler_table[irq_num](NULL);
	disable_irq();
}


void irq_init()
{
	register_irq(timer4_irq_handler, TIMER4_IRQ_NUM, 0);
}