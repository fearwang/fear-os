#include <proc.h>
//"msr spsr, #(0x80|0x40|0x10)\n\t"
void go_pc(unsigned int pc, unsigned int sp)
{
	asm volatile(
		"stmfd sp!, {%1}\n\t"
		"ldmfd sp!, {r13}^\n\t"
		"stmfd sp!, {%0}\n\t"
		"msr spsr, #(0x40|0x10)\n\t"
		"ldmfd sp!, {pc}^\n\t"	
		:
		:"r"(pc), "r"(sp)
		:"r8"
	);
}


int exec(unsigned int start){
	struct task_info *addr = (struct task_info*)kmalloc(TASK_SIZE);
	
	unsigned int sp_t = ((unsigned int)addr + (TASK_SIZE));
	printk("task_init,get task_info addr:%x\n",sp_t);
	//printk("get stack addr:%x\n",sp_t);
	
	//printk("get sp = %x\n", get_sp());
	//set_sp(((unsigned int)addr + (TASK_SIZE-1)));
	//printk("after set sp get sp = %x\n", get_sp());
	
	go_pc(start, sp_t);

	return 0;

}

