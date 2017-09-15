
#define DISABLE_IRQ 0x80
#define DISABLE_FIQ 0x40
#define SYS_MOD 	0x1f
#define IRQ_MOD 	0x12
#define FIQ_MOD 	0x11
#define SVC_MOD 	0x13
#define ABT_MOD 	0x17
#define USR_MOD 	0x10
#define UND_MOD 	0x1b
#define MOD_MASK 	0x1f


void switch_to_user_mode()
{
	asm volatile (
		"msr cpsr_c, #(0x80|0x10)\n\t"
		:::"r4"
	);
}