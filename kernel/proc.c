/*
proc.c:
Copyright (C) 2009  david leels <davidontech@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.
*/

#include<kmalloc.h>
#include<proc.h>




struct task_info *current_task_info(void)
{
	register unsigned long sp asm ("sp");
	printk("sp = %x\n", sp);
	return (struct task_info *)(sp&~(TASK_SIZE-1));
}

#define current	current_task_info()

//目前用户空间和内核空间用同一个堆栈
int task_init(void){
	//struct task_info *addr = (struct task_info*)kmalloc(TASK_SIZE);
	//printk("task_init,get task_info addr:%x\n",addr);
	//unsigned int sp_t = ((unsigned int)addr + (TASK_SIZE-1));
	//printk("task_init,get stack addr:%x\n",sp_t);
	//set_sp(sp_t);
	//printk("after set sp\n");
	current->next = current;
	return 0;
}

#define disable_schedule(x)	disable_irq()
#define enable_schedule(x)	enable_irq()

int task_stack_base=0x30300000;
struct task_info *copy_task_info(struct task_info *tsk)
{
	struct task_info *addr = (struct task_info*)kmalloc(TASK_SIZE);
	unsigned int sp_t = ((unsigned int)addr + (TASK_SIZE-1));
	//struct task_info *tmp=(struct task_info *)task_stack_base;
	//task_stack_base+=TASK_SIZE;
	return addr;
}


#define DO_INIT_SP(sp,fn,args,lr,cpsr,pt_base)	\
		do{\
				(sp)=(sp)-4;/*r15*/		\
				*(volatile unsigned int *)(sp)=(unsigned int)(fn);/*r15*/		\
				(sp)=(sp)-4;/*r14*/		\
				*(volatile unsigned int *)(sp)=(unsigned int)(lr);/*r14*/		\
				(sp)=(sp)-4*13;/*r12,r11,r10,r9,r8,r7,r6,r5,r4,r3,r2,r1,r0*/		\
				*(volatile unsigned int *)(sp)=(unsigned int)(args);\
				(sp)=(sp)-4;/*cpsr*/		\
				*(volatile unsigned int *)(sp)=(unsigned int)(cpsr);\
		}while(0)




int do_fork(int (*f)(void *), void *args)
{
	struct task_info *tsk,*tmp;
	if((tsk = copy_task_info(current))== (void *)0 )
		return -1;

	tsk->sp=((unsigned int)(tsk)+TASK_SIZE);
	
	DO_INIT_SP(tsk->sp,f,args,0,0x1f&get_cpsr(),0);

	disable_schedule();
	tmp = current->next;
	current->next = tsk;
	tsk->next = tmp;
	enable_schedule();

	return 0;
}

void *__common_schedule(void){
	return (void *)(current->next);
}
