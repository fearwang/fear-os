OUTPUT_ARCH(arm)
ENTRY(_start)

SECTIONS
{
	. = 0x30000000;
    __ro_start__ = .;
	.text : 
	{
		*(.text)
	}
    __ro_end__ = .;

	. = ALIGN(32);						
    __rw_start__ = .;
	.data : 
	{
		*(.data)
	}
    __rw_end__ = .;


	.rodata : 
	{
		*(.rodata*)
	}


	. = ALIGN(32);
	__bss_start__	= .;
	.bss : 
	{
		*(.bss)
	}
	__bss_end__ = .;
}
