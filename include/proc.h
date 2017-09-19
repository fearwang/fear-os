#ifndef __PROC_H__
#define __PROC_H__

struct task_info{
	unsigned int sp;
	struct task_info *next;
};

#define TASK_SIZE	8192

static inline unsigned int get_cpsr(void)
{
	unsigned int p;
	asm volatile(
		"mrs %0,cpsr\n"
		:"=r"(p)
		:
	);
	return p;
}

static inline unsigned int get_sp(void)
{
	unsigned int p;
	asm volatile(
		"mov %0,sp\n"
		:"=r"(p)
		:
	);
	return p;
}


//根据arm c 调用准则 r0 = sp
static inline unsigned int set_sp(unsigned int sp)
{
	//printk("start of set sp\n");
	//unsigned int p = sp;
	asm volatile(
		"mov sp, %0\n"
		"stmfd sp!,{lr}\n"
		:		
		:"r"(sp)
		:"r1"
	);
	//printk("end of set sp\n");
	return 0;
}

//根据arm c 调用准则 r0 = sp
static inline unsigned int set_sp_usr(unsigned int sp)
{
	
	//unsigned int p = sp;
	asm volatile(
		"mov sp, r0\n"
		:		
		:
		:"r0", "r1"
	);
	
	return 0;
}

#endif