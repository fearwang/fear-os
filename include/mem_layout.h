/*
 *  ----------------    0x30000000
 *  |  kernel img  |
 *  |--------------|    0x30200000  //2M for kernel img
 *  |              |
 *  |			   |
 *  |	  heap     |	
 *  |		       |
 *  |			   |
 *  |--------------|
 *  |	meta data  |
    |   for buddy  |
	|    system    | 
 *  |--------------|     0x33000000
 *  |	page table |
 *  |--------------|     0x33a00000
 *  |			   |
 *  | system stack |
 *  |   		   |
 *  |--------------|     0x34000000
 *  
 *
 */


#ifndef __MEM_LAYOUT_H_
#define __MEM_LAYOUT_H_

#define PLAT_MEM_SIZE			0x4000000
#define PLAT_MEM_START			0x30000000

#define KERNEL_TEXT_BASE	0x30000000
#define KERNEL_IMG_MAX_SIZE 0x200000

#define SVC_STACK_SIZE   0x100000
#define IRQ_STACK_SIZE   0x100000
#define FIQ_STACK_SIZE   0x100000
#define ABT_STACK_SIZE   0x100000
#define UND_STACK_SIZE   0x100000
#define SYS_STACK_SIZE   0x100000


#define SVC_STACK		( PLAT_MEM_START + PLAT_MEM_SIZE )
#define IRQ_STACK		( SVC_STACK-SVC_STACK_SIZE)
#define FIQ_STACK		( IRQ_STACK-IRQ_STACK_SIZE)
#define ABT_STACK		( FIQ_STACK-FIQ_STACK_SIZE)
#define UND_STACK		( ABT_STACK-ABT_STACK_SIZE)
#define SYS_STACK		( UND_STACK-UND_STACK_SIZE)

#define PAGE_TABLE_SIZE   0xa00000  //10M
#define PAGE_TABLE_BASE_ADDR SYS_STACK - SYS_STACK_SIZE - PAGE_TABLE_SIZE

#define PLAT_HEAP_MEM_START PLAT_MEM_START + KERNEL_IMG_MAX_SIZE
#define PLAT_HEAP_MEM_END PAGE_TABLE_BASE_ADDR

#endif
