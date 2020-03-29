#include "io.h"
#include "util.h"

extern unsigned char _end;
int loadimg(){
    char *addr;

    print("input size[@loc]: ");

    char addr_mode;
    int size = get_nature(&addr_mode, 10, 1);
    if(addr_mode == '@')
        addr = (char*)get_nature(0, 16, 1);
    else
        addr = (char*)&_end;

    char *ptr = addr;

    printf(NEWLINE "load %d bytes @0x%x" NEWLINE, size, (ULL)ptr);
    println("please input image now...");

    while(size--){
        char c = getchar();
        *ptr++ = c;
#if 0
        print("load@ 0x");
        _print_ULL_as_number((ULL)(ptr - 1), 16);
        puts("");
        if(size % (2<<10) == 0){
            //println("remain... ", size, " bytes");
            println("remain... ");
        }
#endif
    }
    if(expect("|GMIDAOL")){
        puts("LOAD IMG DONE!!");
        printf("jump to 0x%x" NEWLINE, addr);
        __asm__ volatile("br %0" :: "r"(addr));
        //__asm__ volatile("b _new_kernel");
        //__asm__ volatile ("b _start");
        return 1;
    }
    else{ puts("IMG POSTFIX FAILED"); }
    return 0;
}
