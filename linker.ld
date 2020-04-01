/*
SECTIONS
{
    . = 0x80000;
    __start_begin = .;
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
    .bootloader : {
        __start_bootloader = . ;
        *(.bootloader) ;
        __stop_bootloader = . ;
    }
    _end = .;
}
__bss_size = (__bss_end - __bss_start)>>3;
__loader_size = (_end - _code)>>3;
*/

SECTIONS
{
    . = 0x80000;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { _bstart = ALIGN(0x10) ; *(.bss) ; _bend = ALIGN(0x10) ; }
    .bootloader : {
        __start_bootloader = . ;
        *(.bootloader) ;
        __stop_bootloader = . ;
    }
}
    

/*
在採用段式內存管理的架構中，BSS段（bss segment）通常是指用來存放程序中未初始化的全局變量的一塊內存區域。
BSS是英文Block Started by Symbol的簡稱。
BSS段屬於靜態內存分配。.bss section 的空間結構類似於 stack
*/
