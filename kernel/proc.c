#include<kmalloc.h>
#include<proc.h>


struct task_info *current_task_info(void)
{
	register unsigned long sp asm ("sp");
	return (struct task_info *)((sp-1)&~(TASK_SIZE-1));
}

#define current	current_task_info()

//mem layout静态划分的1M stack 给swapper使用  task_info
int task_init(void){

	current->next = current;
	return 0;
}

#define disable_schedule(x)	disable_irq()
#define enable_schedule(x)	enable_irq()

struct task_info *copy_task_info(struct task_info *tsk)
{
	struct task_info *addr = (struct task_info*)kmalloc(TASK_SIZE);
	unsigned int sp_t = ((unsigned int)addr + (TASK_SIZE));
	return addr;
}


#define DO_INIT_SP(sp,user_sp,fn,args,lr,cpsr,pt_base)	\
		do{\
				(sp)=(sp)-4;/*spsr*/		\
				*(volatile unsigned int *)(sp)=(unsigned int)(cpsr);\
				(sp)=(sp)-4;/*r15*/		\
				*(volatile unsigned int *)(sp)=(unsigned int)(fn);/*r15*/		\
				(sp)=(sp)-4;/*r14*/		\
				*(volatile unsigned int *)(sp)=(unsigned int)(lr);/*r14*/		\
				(sp)=(sp)-4;/*r13*/		\
				*(volatile unsigned int *)(sp)=(unsigned int)(user_sp);/*r13*/		\
				(sp)=(sp)-4*13;/*r12,r11,r10,r9,r8,r7,r6,r5,r4,r3,r2,r1,r0*/		\
				*(volatile unsigned int *)(sp)=(unsigned int)(args);\
		}while(0)




void kthread_daemon1(void* args)
{
	struct task_info* next = current->next;
	
	while(1) {
		printk("kthread_daemon1: sp = %x, cpsr = %x\n",get_sp(), get_cpsr());
		volatile unsigned int time=0xffff;
		while(time--);
	}
	
}
			
int do_fork(int (*f)(void *), void *args)
{
	struct task_info *tsk,*tmp;
	tsk = (struct task_info*)kmalloc(TASK_SIZE);//内核堆栈
	printk("do_fork, tsk=%x\n",tsk);
	if(tsk == 0)
		return -1;
	
	void* user_sp = kmalloc(TASK_SIZE);		//user mode 堆栈
	user_sp = (void*)((unsigned int)user_sp + (TASK_SIZE));
	printk("do_fork, user_sp=%x\n",user_sp);
	unsigned int kernel_stack = ((unsigned int)tsk+TASK_SIZE);

	tsk->context_save.r4 = 0;
	tsk->context_save.r5 = 0;
	tsk->context_save.r6 = 0;
	tsk->context_save.r7 = 0;
	tsk->context_save.r8 = 0;
	tsk->context_save.r9 = 0;
	tsk->context_save.r10 = 0;
	tsk->context_save.r11 = 0;
	tsk->context_save.r12 = 0;
	tsk->context_save.sp = ((unsigned int)tsk)+TASK_SIZE-68;  //初始化内核栈的sp，预留出user mode的现场，假装完成一次irq返回
	extern int ret_from_fork;
	unsigned int *p = &ret_from_fork;
	tsk->context_save.lr = (unsigned int)p;
	tsk->context_save.cpsr = 0x53;
	printk("tsk->context_save.lr = %x\n", tsk->context_save.lr);
	printk("tsk->context_save.cpsr = %x\n", tsk->context_save.cpsr);
	
	DO_INIT_SP(kernel_stack, user_sp, f, args, 0, 0x50, 0);

	disable_schedule();
	tmp = current->next;		//task_info加入链表 合适的时机会发生context switch
	current->next = tsk;
	tsk->next = tmp;
	enable_schedule();

	return 0;
}

int kthread_create(int (*f)(void *), void *args)
{
	struct task_info *tsk,*tmp;
	tsk = (struct task_info*)kmalloc(TASK_SIZE);//内核堆栈
	printk("do_fork, kernel tsk=%x\n",tsk);
	if(tsk == 0)
		return -1;

	tsk->context_save.r4 = 0;
	tsk->context_save.r5 = 0;
	tsk->context_save.r6 = 0;
	tsk->context_save.r7 = 0;
	tsk->context_save.r8 = 0;
	tsk->context_save.r9 = 0;
	tsk->context_save.r10 = 0;
	tsk->context_save.r11 = 0;
	tsk->context_save.r12 = 0;
	tsk->context_save.sp = ((unsigned int)tsk)+TASK_SIZE;  //初始化内核栈的sp，预留出user mode的现场，假装完成一次irq返回
	
	tsk->context_save.lr = (unsigned int)f;
	tsk->context_save.cpsr = 0x53;
	printk("tsk->context_save.lr = %x\n", tsk->context_save.lr);
	printk("tsk->context_save.cpsr = %x\n", tsk->context_save.cpsr);

	disable_schedule();
	tmp = current->next;		//task_info加入链表 合适的时机会发生context switch
	current->next = tsk;
	tsk->next = tmp;
	enable_schedule();

	return 0;
}


void switch_mm()
{
	
}

void __asm_switch_to(struct task_info* prev, struct task_info* next)
{		
	asm volatile(
		"stmia %0!,{r4-r12,sp,lr}\n\t" //store context
		"mrs r4, cpsr\n\t"				  
		"stmia %0!,{r4}\n\t"
		
		"ldr r4,[%1,#44]\n\t"		//r4 = cpsr
		"msr spsr_cxsf,r4\n\t"
		"ldmia %1!,{r4-r12,sp,pc}^\n\t"
		:
		:"r"(prev),"r"(next)
	);
}

void *__common_schedule(void){
	return (void *)(current->next);
}

void context_switch(struct task_info* prev, struct task_info* next)
{
	printk("prev = %x, next = %x\n", prev, next);
	switch_mm();
	__asm_switch_to(prev, next);
}

void schedule()
{
	struct task_info* prev = current;
	struct task_info* next = __common_schedule();
	if((unsigned int)prev == (unsigned int)next) {
		printk("schedule():next == prev \n");
		return;
	}
	
	printk("before context_switch, sp = %x, cpsr = %x\n",get_sp(),get_cpsr());
	context_switch(prev, next);
	printk("after context_switch, sp = %x, cpsr = %x\n",get_sp(),get_cpsr());
}
