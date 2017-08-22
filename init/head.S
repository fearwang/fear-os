.extern main
.text
.global _start
_start:
Reset:
    ldr sp, =4096
    bl disable_watch_dog
    
    bl clk_init
    bl memsetup
    ldr pc, =on_sdram
on_sdram:
    ldr sp, 0x34000000
    ldr lr, halt_loop
    ldr pc, =main

halt_loop:
    b halt_loop
