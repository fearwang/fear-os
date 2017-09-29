#ifndef __PROC_H__
#define __PROC_H__

struct cpu_context_save {
	unsigned int r4;
	unsigned int r5;
	unsigned int r6;
	unsigned int r7;
	unsigned int r8;
	unsigned int r9;
	unsigned int r10;
	unsigned int r11;
	unsigned int r12;
	unsigned int sp;
	unsigned int lr;
	//unsigned int pc;
	unsigned int cpsr;
};

struct task_info{
	//unsigned int sp;
	struct cpu_context_save context_save;
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
static inline void set_sp(unsigned int sp)
{
	//printk("start of set sp\n");
	//unsigned int p = sp;
	asm volatile(
		"mov sp, %0\n"
		
		:		
		:"r"(sp)
	);
	//printk("end of set sp\n");
	//return 0;
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