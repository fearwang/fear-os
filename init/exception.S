
.equ DISABLE_IRQ,0x80
.equ DISABLE_FIQ,0x40
.equ SYS_MOD,0x1f
.equ IRQ_MOD,0x12
.equ FIQ_MOD,0x11
.equ SVC_MOD,0x13
.equ ABT_MOD,0x17
.equ UND_MOD,0x1b
.equ MOD_MASK,0x1f


.macro CHANGE_TO_SVC
    msr     cpsr_c,#(DISABLE_FIQ|DISABLE_IRQ|SVC_MOD)
.endm

.macro CHANGE_TO_IRQ
    msr     cpsr_c,#(DISABLE_FIQ|DISABLE_IRQ|IRQ_MOD)
.endm


.global	__vector_undefined
.global	__vector_swi
.global	__vector_prefetch_abort
.global	__vector_data_abort
.global	__vector_reserved
.global	__vector_irq
.global	__vector_fiq

.text
.code 32

__vector_undefined:
	nop
__vector_swi:
	nop
__vector_prefetch_abort:	
	nop
__vector_data_abort:
	nop
__vector_reserved:
	nop
__vector_irq:		
	sub r14,r14,#4			//计算上一个状态的返回地址
	str r14,[r13,#-0x4]		//并将其保存
	mrs r14,spsr			//spsr_irq保存到r14中
	str r14,[r13,#-0x8]		//压栈，即将上一个状态的cpsr保存起来 即使再发生中断 lr和cpsr都不会被破坏
	str r0,[r13,#-0xc]		//在irq模式的堆栈中保存r0
	mov r0,r13				//使用r0保存r13的值给svc模式  svc模式从中取得lr 和spsr_irq
	CHANGE_TO_SVC		
	str r14,[r13,#-0x8]  	//同样是保存r14_svc
	ldr r14,[r0,#-0x4]		//设置r14_svc为r0传递过来的值 即r13_irq
	str r14,[r13,#-0x4]		//从r13_irq回复出上一个状态的返回地址

    sub r13,r13,#0x8

	ldr r14,[r0,#-0x8]
	ldr r0,[r0,#-0xc]
	stmdb r13!,{r0-r3,r14}
	bl common_irq_handler
	ldmia r13!,{r0-r3,r14}
	msr spsr,r14
	ldmfd r13!,{r14,pc}^
	
__vector_fiq:
	nop

