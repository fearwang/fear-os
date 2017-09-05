.text
.extern disable_watch_dog
.extern clock_init
.extern memsetup
.extern nand_init_boot
.extern uart0_init_boot
.extern relocate_img_to_dram

.global _start
_start:
/*
device nand0 <nandflash0>, # parts = 4
#: name                        size            offset          mask_flags
0: bootloader          0x00040000      0x00000000      0
1: params              0x00020000      0x00040000      0
2: kernel              0x00200000      0x00060000      0
3: root                0x0fda0000      0x00260000      0
*/
    ldr sp, =4096
    bl disable_watch_dog
    bl clock_init
    
    //enable icache
    mrc p15, 0, r0, c1, c0, 0   // read control reg
    orr r0, r0, #(1<<12)
    mcr p15, 0, r0, c1, c0, 0   // write it back

    bl memsetup
    ldr sp, =0x34000000

    bl nand_init_boot
    bl uart0_init_boot
    //relocate boot itself to 0x30000000
    mov r0, #0          //src
    ldr r1, =0x33000000 //dst
    ldr r2, _boot_bss_start_
    sub r2, r2, r1     //len

#    bl copy_steppingstone_to_sdram
    bl relocate_img_to_dram
#    bl compare_dram_and_sram_4k
    ldr pc, =on_sdram
on_sdram:
    //copy kernel from flash and jump to it entry (0x30040000)
    ldr r0, _kernel_offset_nand //src
    ldr r1, _kernel_entry_addr  //dst
    ldr r2, _kernel_size_nand   //len
    bl copy_kernel_from_nand
    ldr pc, _kernel_entry_addr 
    ldr lr, =halt_loop 
    

halt_loop:
    b halt_loop

_boot_bss_start_:.word __rodata_end__
_kernel_offset_nand:.word 0x00060000
_kernel_size_nand:.word 0x00200000
_kernel_entry_addr:.word 0x30000000
