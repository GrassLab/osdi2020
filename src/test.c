#include "io.h"

int bss_all_be(){
    int *beg, *end;
#ifdef SHOW
    println("SHOW ALL BSS VALUE");
#endif
    __asm__ volatile ("adr %0, __bss_beg" : "=r"(beg));
    __asm__ volatile ("adr %0, __bss_end" : "=r"(end));
    for(int *p = beg; p != end; p++){
#ifdef SHOW
        print("0x");
        _print_ULL_as_number((unsigned long long)p, 16);
        println(": ", (int)*p);
#endif
        if(*p != 0) return 1;
    }
    return 0;
}

extern char __stack_top;
int *stack_pointer(){
    int *sp; 
    __asm__ volatile ("mov %0, sp" : "=r"(sp));
#ifdef SHOW
    print("sp @0x");
    _print_ULL_as_number((unsigned long long)sp, 16);
    puts("");
    print("__stack_top @0x");
    _print_ULL_as_number((unsigned long long)&__stack_top, 16);
    puts("");
#endif
    return sp;
}
