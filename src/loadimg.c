#include "io.h"
#include "util.h"

extern unsigned char _kbeg;
extern unsigned char _kend;
extern unsigned char _bootloader_beg;

char *nkaddr;
unsigned int nksize;

void loadimg(){

    print("input size[@loc]: ");

    char addr_mode;
    nksize = get_nature(&addr_mode, 10, 1);

    if(addr_mode == '@')
        nkaddr = (char*)get_nature(0, 16, 1);
    else
        nkaddr = (char*)&_kend;

    puts("");

    /* copy the current kernel */
    char *okbeg = (char*)&_kbeg,
         //*nkbeg = (char*)0x80000;
         *nkbeg = (char*)0x200000;
    char *okptr = okbeg,
         *nkptr = nkbeg;
         
    char *bootloader_beg = (char*)&_bootloader_beg;
    char *addr = bootloader_beg - okbeg + nkbeg + 8;

    print("move old kernel code...");
    while(okptr != (char*)&_kend){
        *nkptr = *okptr;
        nkptr++, okptr++;
    }
    puts("done");
    printf("okbeg  0x%x" NEWLINE, okbeg);
    printf("nkbeg  0x%x" NEWLINE, nkbeg);
    printf("loader  0x%x" NEWLINE, bootloader_beg);
    printf("jump to 0x%x" NEWLINE, addr);
    print("jump to new location of old kernel...");
    __asm__ volatile("br %0" :: "r"(addr)); 
}

void __attribute__((__section__(".bootloader"))) copy_and_load(){
    puts("done");
    char *ptr = nkaddr;
    printf(NEWLINE "load %d bytes @0x%x" NEWLINE, nksize, (ULL)ptr);
    println("please input image now...");
    while(nksize--){
        char c = getchar();
        *ptr++ = c;
    }
    __asm__ volatile("br %0" :: "r"(nkaddr));
}
