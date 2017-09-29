#include <proc.h>
//"msr spsr, #(0x80|0x40|0x10)\n\t"
void go_pc(unsigned int pc, unsigned int sp_u, unsigned int sp_k)
{
	asm volatile(
		"mov sp, %2\n\t"
		"stmfd sp!, {%1}\n\t"
		"ldmfd sp!, {r13}^\n\t"
		"stmfd sp!, {%0}\n\t"
		"msr spsr, #(0x40|0x10)\n\t"
		"ldmfd sp!, {pc}^\n\t"
		:
		:"r"(pc), "r"(sp_u), "r"(sp_k)
	);
}


int exec(unsigned int start){
	struct task_info *addr_u = (struct task_info*)kmalloc(TASK_SIZE);
	struct task_info *addr_k = (struct task_info*)kmalloc(TASK_SIZE);
	
	unsigned int sp_u = ((unsigned int)addr_u + (TASK_SIZE));
	unsigned int sp_k = ((unsigned int)addr_k + (TASK_SIZE));
	//printk("task_init,get task_info addr:%x\n",sp_t);
	printk("get stack addr_u:%x\n",sp_u);
	printk("get stack addr_k:%x\n",sp_k);
	
	//printk("get sp = %x\n", get_sp());
	//set_sp(((unsigned int)addr + (TASK_SIZE-1)));
	//printk("after set sp get sp = %x\n", get_sp());
	
	go_pc(start, sp_u, sp_k);

	return 0;

}


