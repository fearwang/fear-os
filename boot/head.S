.extern _start
.text
//.global _start
//_start:
Reset:
    ldr sp, =4096
    bl disable_watch_dog
    bl clock_init
    bl memsetup
    ldr sp, =0x34000000

    bl nand_init
    bl uart0_init
    bl puts_hello

   // mov r0, #0
//    ldr r1, =0x30000000
 //   ldr r2, = __bss_start__
 //   sub r2, r2, r1
  //  bl copy_code_to_sdram

    bl copy_steppingstone_to_sdram
    bl compare_dram_and_sram_4k
    ldr pc, =on_sdram
on_sdram:
    ldr lr, =halt_loop
    ldr pc, =_start

halt_loop:
    b halt_loop

