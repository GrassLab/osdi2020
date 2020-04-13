#include "io.h"
#include "map.h"
#define nb8p(bytes, n, p) (((1 << n) - 1) & (bytes >> (p)))

  
void exception_handler(){
    //char *ret;
    //unsigned long long esr;
    //__asm__ volatile ("mrs %0, ELR_EL2 " : "=r"(ret));
    //printf("exc ret adr = 0x%x" NEWLINE, ret);
    //__asm__ volatile ("mrs %0, ESR_EL2 " : "=r"(esr));
    //printf("esr reg val = 0x%x" NEWLINE, esr);
    puts("Exceptions Handler");
}

void synchronous_exceptions(){
    char *ret;
    unsigned long esr;
    unsigned long current_el;
    __asm__ volatile("mrs %0, CurrentEL\n\t" : "=r" (current_el) : : "memory");
    printf("currentEL: %d" NEWLINE, current_el >> 2);

#ifdef RUN_ON_EL2
    __asm__ volatile ("mrs %0, ELR_EL2" : "=r"(ret));
    __asm__ volatile ("mrs %0, ESR_EL2" : "=r"(esr));
#else
    __asm__ volatile ("mrs %0, ELR_EL1" : "=r"(ret));
    __asm__ volatile ("mrs %0, ESR_EL1" : "=r"(esr));
#endif

    printf("Exception return address 0x%x" NEWLINE, ret);
    printf("Exception class (EC) 0x%x" NEWLINE, nb8p(esr, 6, 26));
    printf("Instruction spepcfic syndrome (ISS) 0x%x" NEWLINE, nb8p(esr, 25, 0));
    puts("Synchronous Exceptions");
    int *sp;
    __asm__ volatile ("mov %0, sp" : "=r"(sp));
    printf("sp = 0x%x" NEWLINE, sp);

#define MOV_GPR(x) __asm__ volatile ("mov x" #x ", #9487");

    MAP(MOV_GPR, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30);
}
