SECTIONS{
	. = 0x80000;
	.text : { KEEP(*(.text.assembly)) *(.text .text.* .gnu.linkonce.t*) }
	.rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
	PROVIDE(_data = .);
	.data : { *(.data .data.* .gnu.linkonce.d*) }
	.bss (NOLOAD): {
		. = ALIGN(16);
		__bss_start = .;
		*(.bss .bss.*)
		*(COMMON)
		__bss_end = .;
	}
	. = ALIGN(32);
	_stack_bottom = .;
	. = . + 0x8000;
	_stack_top = .;
	_heap_bottom = .;
	_heap_top = 0x20008000;
}
__bss_size = (__bss_end - __bss_start)>>3;
