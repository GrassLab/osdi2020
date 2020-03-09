SECTIONS    /* defines where the code is going to be stored in memory */
{
    . = 0x80000;    /* the kernel must be stored at 0x80000 in ram, because lower addresses are reserved */
    .text : { *(.text) } /* maps the text segment from the assembly file */
}
