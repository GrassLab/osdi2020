SECTIONS
{
    . = 0x80000;
    __start_begin = .;
    /* . = 0x80000;*/
    PROVIDE(_code = .);
    .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    PROVIDE(_data = .);
    .data : { *(.data .data.* .gnu.linkonce.d*) }
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    _end = .;
}
__bss_size = (__bss_end - __bss_start)>>3;
__loader_size = (_end - _code)>>3;

/*
在採用段式內存管理的架構中，BSS段（bss segment）通常是指用來存放程序中未初始化的全局變量的一塊內存區域。
BSS是英文Block Started by Symbol的簡稱。
BSS段屬於靜態內存分配。.bss section 的空間結構類似於 stack
*/
