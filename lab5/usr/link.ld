SECTIONS
{
	. = 0x0;
	.text.entry : {*(text.entry) }
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : {*(.data)}
	.bss : {
		. = ALIGN(16);
		*(.bss*)
	}
}