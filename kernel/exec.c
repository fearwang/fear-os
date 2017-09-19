#include <proc.h>

void go_pc(unsigned int pc, unsigned int sp)
{
	asm volatile(
		"mov sp,%1\n\t"
		"mov pc,%0\n\t"
		
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

