
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

.macro CHANGE_TO_SYS
    msr     cpsr_c,#(DISABLE_FIQ|DISABLE_IRQ|SYS_MOD)
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
	
__vector_swi:			//软中断会使得cpu进入svc mode 单核不考虑swi重入?
	stmfd sp!, {r0-r3,r12,r14}
	//mov r1, sp
	mrs r0, spsr
	stmfd sp!, {r0}

	ldr r0, [lr, #-4]
	bic r0, r0, #0xff000000
	
	ldr r12, [sp, #4] 		//sp指向spsr，+4位置保存r0，根据r0取出参数
	ldr r1, [r12, #4]
	ldr r2, [r12, #8]
	
	bl	sys_call_schedule
	ldr r12, [sp, #4]
	str r0, [r12]		//返回值保存在之前预留的位置中
	
	mov r3, r0
	mov r0, #0
	mov r1, r3	
	bl print_reg
	
	
	
	ldmfd sp!, {r12}		//以下两条指令先恢复spsr
	msr spsr, r12
	ldmfd sp!, {r0-r3, r12, pc}^
	
__vector_prefetch_abort:	
	nop
	
__vector_data_abort:
	nop
	
__vector_reserved:
	nop
	
__vector_irq:		//中断发生的时候 硬件自动禁止中断
	sub r14,r14,#4
	stmfd r13!,{r14}
	mrs r14,spsr
	stmfd r13!,{r14}
	
	CHANGE_TO_SYS
	stmfd r13!,{r0-r3}	//保存被调用者不会保存的寄存器 其他寄存器common_irq_handler函数会保存 所以这里不用保存
	bl common_irq_handler
	ldmfd r13!,{r0-r3}
	
	CHANGE_TO_IRQ
	ldmfd r13!,{r14}
	msr spsr,r14
	ldmfd r13!,{pc}^
	
__vector_fiq:
	nop

