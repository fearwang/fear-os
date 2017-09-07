#include <mem_layout.h>

/*mask for page table base addr*/
#define PAGE_TABLE_L1_BASE_ADDR_MASK	(0xffffc000)

#define VIRT_TO_PTE_L1_INDEX(addr)	(((addr)&0xfff00000)>>18)

//config MACRO
#define PTE_L1_SECTION_NO_CACHE_AND_WB	(0x0<<2)
#define PTE_L1_SECTION_DOMAIN_DEFAULT	(0x0<<5)
#define PTE_ALL_AP_L1_SECTION_DEFAULT	(0x1<<10)

#define PTE_L1_SECTION_PADDR_BASE_MASK	(0xfff00000)  //mask select page entry offset
#define PTE_BITS_L1_SECTION				(0x2)

//let page tabe stay at the high mem size = 10M   we can map 10*1024*1024/4 pages
#define L1_PTR_BASE_ADDR			PAGE_TABLE_BASE_ADDR	

#define PHYSICAL_MEM_ADDR			0x30000000 //start of paddr in jz2440
#define VIRTUAL_MEM_ADDR			0x30000000 //now we make va = pa
#define MEM_MAP_SIZE				0x4000000   //total size of mem

#define PHYSICAL_IO_ADDR			0x48000000	// start paddr of IO REG
#define VIRTUAL_IO_ADDR				0x48000000	// va of IO addr we want to map
#define IO_MAP_SIZE					0x18000000	// size of io addr to map

#define VIRTUAL_VECTOR_ADDR 0x0             // start va of vector table
#define PHYSICAL_VECTOR_ADDR 0x30000000     //start pa of vector table
#define VECTOR_MAP_SIZE 0x100000

void start_mmu(void){
	unsigned int ttb=L1_PTR_BASE_ADDR;

	asm (
		"mcr p15,0,%0,c2,c0,0\n"    /* set base address of page table*/
		"mvn r0,#0\n"                  
		"mcr p15,0,r0,c3,c0,0\n"    /* enable all region access*/

		"mov r0,#0x1\n"
		"mcr p15,0,r0,c1,c0,0\n"    /* set back to control register */
		"mov r0,r0\n"
		"mov r0,r0\n"
		"mov r0,r0\n"
		:
		: "r" (ttb)
		:"r0"
	);
    printk("end of start mmu\n\r");
}

unsigned int gen_l1_pte(unsigned int paddr){
	return (paddr&PTE_L1_SECTION_PADDR_BASE_MASK)|\
										PTE_BITS_L1_SECTION;
}

//compute the index addr in mmu table by va 
//then we could write data to the index addr(paddr & attr)
unsigned int gen_l1_pte_addr(unsigned int baddr,\
										unsigned int vaddr){
	return (baddr&PAGE_TABLE_L1_BASE_ADDR_MASK)|\
								VIRT_TO_PTE_L1_INDEX(vaddr);
}

void init_sys_mmu(void){
	unsigned int pte;
	unsigned int pte_addr;
	int j;
    //map first 1M space of kernel img  to 0x00000000
	for(j = 0;j < VECTOR_MAP_SIZE>>20; j++){
		pte=gen_l1_pte(PHYSICAL_VECTOR_ADDR+(j<<20)); //every 1M space from 0x30000000
		pte|=PTE_ALL_AP_L1_SECTION_DEFAULT;
		pte|=PTE_L1_SECTION_NO_CACHE_AND_WB;
		pte|=PTE_L1_SECTION_DOMAIN_DEFAULT;
		pte_addr=gen_l1_pte_addr(L1_PTR_BASE_ADDR,\
								VIRTUAL_VECTOR_ADDR+(j<<20));
		*(volatile unsigned int *)pte_addr=pte;
	}

	for(j = 0;j < MEM_MAP_SIZE>>20; j++){
		pte=gen_l1_pte(PHYSICAL_MEM_ADDR+(j<<20)); //every 1M space from 0x30000000
		pte|=PTE_ALL_AP_L1_SECTION_DEFAULT;
		pte|=PTE_L1_SECTION_NO_CACHE_AND_WB;
		pte|=PTE_L1_SECTION_DOMAIN_DEFAULT;
		pte_addr=gen_l1_pte_addr(L1_PTR_BASE_ADDR,\
								VIRTUAL_MEM_ADDR+(j<<20));
		*(volatile unsigned int *)pte_addr=pte;
	}

	for(j = 0;j < IO_MAP_SIZE>>20; j++){
		pte=gen_l1_pte(PHYSICAL_IO_ADDR+(j<<20));
		pte|=PTE_ALL_AP_L1_SECTION_DEFAULT;
		pte|=PTE_L1_SECTION_NO_CACHE_AND_WB;
		pte|=PTE_L1_SECTION_DOMAIN_DEFAULT;
		pte_addr=gen_l1_pte_addr(L1_PTR_BASE_ADDR,\
								VIRTUAL_IO_ADDR+(j<<20));
		*(volatile unsigned int *)pte_addr=pte;
	}
    printk("endof init_sys_mmu\n\r");
}
