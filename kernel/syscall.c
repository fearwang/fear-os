#include <syscall.h>

void test_syscall_args(int index,int *array){
	printk("this following message is from kernel printed by test_syscall_args\n");
	int i;
	for(i = 0;i < index; i++){
		printk("the %d arg is %x\n",i,array[i]);
	}
}

void print_reg(unsigned int reg_num, unsigned int reg)
{
	printk("r%d = %d\n", reg_num, reg);
}

int __syscall_test(int index,int *array) {
	test_syscall_args(index, array);
	return 8;
}

int  __syscall_malloc(int index, int *array) {
	void* addr = kmalloc(array[0]);
	printk("malloc %d , addr : %x\n", array[0], addr);
	return (int)addr;
	//return 2;
}


syscall_fn syscall_table[__NR_SYS_CALL]={
	(syscall_fn)__syscall_test,
	(syscall_fn)__syscall_malloc,
};

int sys_call_schedule(unsigned int index,int num,int *args){
	if(syscall_table[index]){
		return (syscall_table[index])(num,args);
	}
	return -1;
}
