SECTIONS
{
    . = 0x0000000000000000;
    .text.entry : 
	{
		*(text.entry) 
	}
    __text_start = .;
    .text :
    {
        *(.text)
    }
    __text_end = .;
    __rodata_start = .;
    .rodata :
    {
        *(.rodata)
    }
    __rodata_end = .;
    __data_start = .;
    .data :
    {
        *(.data)
    }
    __data_end = .;
    __bss_start = .;
    .bss :
    {
        bss = .;
        *(.bss)
    }
    __bss_end = .;
}