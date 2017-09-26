
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
	
__vector_irq:				   //中断发生的时候 硬件自动禁止中断
	sub r14,r14,#4			   //中断返回地址
	sub sp,sp,#12	
	stmia  sp, {r0,lr}         //save r0, lr
    mrs    lr, spsr 
    str    lr, [sp,#8]         // save spsr
	
	mov r0, sp				   //将irq mode的stack point通过r0传递给即将跳转的函数
	add sp,sp,#12			   //还原irq mode的stack
	
	and lr, lr, #0x1f	       //lr保存了发生IRQ时候的CPSR，通过and操作，可以获取CPSR.M[3:0]的值
	cmp	lr, #0x10
	beq	irq_from_user
	cmp	lr, #0x13
	beq	irq_from_svc
	b .

irq_from_user:
	CHANGE_TO_SVC
	sub sp,sp,#68			//指向r0的位置 最低端
	stmib sp,{r1-r12}		//ib, 留出 r0的位置  sp值不变
	ldmia r0,{r4-r6}		//取出irq模式堆栈中保存的r6 = spsr r5 = lr r4 = r0
	add   r0, sp, #64		//r0指向pc的位置
	str  r4, [sp]			//r0入栈
	stmia r0,{r4-r5}		//入栈pc,spsr
	stmdb r0, {sp,lr}^		//保存usr模式下保存的lr sp  至此user模式下的context保存完毕
	bl common_irq_handler			
	
	
ret_to_user_from_irq:
	ldr    r1, [sp, #64]				//r1保存了pt_regs中的spsr，也就是发生中断时的CPSR 
    ldr    lr, [sp, #60]!    			//lr保存了PC值，同时sp移动到了pt_regs中PC的位置 
    msr    spsr, r1 					//赋值给spsr，进行返回用户空间的准备 spsr_cxsf
										//clrex                    @ clear the exclusive monitor 
    ldmdb  sp, {r0-lr}^ 				//将保存在内核栈上的数据保存到用户态的r0～r14寄存器 
    mov    r0, r0   					//NOP操作，ARMv5T之前的需要这个操作 
    add    sp, sp, #8					//现场已经恢复，移动svc mode的sp到原来的位置 
    movs   pc, lr               		//返回用户空间 
	
irq_from_svc:
	CHANGE_TO_SVC
	sub    sp, sp, #64				//sp指向struct pt_regs中r1的位置 
    stmia  sp, {r1-r12} 			//寄存器入栈。
    ldmia  r0, {r5-r7} 				//取出irq模式堆栈中保存的r7 = spsr r6 = lr r5 = r0
    add    r8, sp, #48				//r8指向struct pt_regs中r13的位置 
									//orig r0设为-1 ; mov    r6, #-1
    add    r4, sp, #64				//r4是发现中断那一刻 svc stack的现场 
    str    r5, [sp,#-4]! 			//保存r0，注意有一个！，sp会加上4，这时候sp就指向栈顶的r0位置了
    mov    r5, lr 					//保存svc mode的lr到r5  ,lr一定要入栈，因为如果svc模式发生中断 lr会被破坏
    stmia  r8,{r4-r7}				//压栈，在栈上形成形成struct pt_regs 
	bl common_irq_handler		
	
ret_to_svc_from_irq:
	ldr    r1, [sp, #64]			//r1保存了pt_regs中的spsr，也就是发生中断时的CPSR 
	msr    spsr, r1					//将中断现场的cpsr值保存到spsr中，准备返回中断发生的现场 spsr_cxsf
    ldmia  sp, {r0-pc}^ 		    //这条指令是ldm异常返回指令，这条指令除了字面上的操作，
	//千万不能写成ldmia  sp!, {r0-pc}^   至于这里为什么能r0-pc(包含sp)，可能指令中的sp只是提供了一个起始地址，因此寄存器列表中可以有sp,因为没有!,所以sp恢复成堆栈中保存的位置
	
__vector_fiq:
	nop

