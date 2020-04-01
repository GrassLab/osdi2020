SECTIONS{
	. = 0x80000;
	.text : { KEEP(*(.text.assembly)) *(.text) }
	.rodata : { *(.rodata) }
	PROVIDE(_data = .);
	.data : { *(.data) }
	.bss (NOLOAD): {
		. = ALIGN(16);
		__bss_start = .;
		*(.bss)
		*(COMMON)
		__bss_end = .;
	}
	. = ALIGN(32);
	_stack_bottom = .;
	. = . + 0x80000;
	_stack_top = .;
}
__bss_size = (__bss_end - __bss_start)>>3;
