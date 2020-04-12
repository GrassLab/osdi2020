#include "syscall.h"

void svc(int num)
{
    //asm volatile("svc %0" :"=r"(num));
    asm volatile("svc #1");
}

void brk(int num)
{
    //asm volatile("svc %0" :"=r"(num));
    asm volatile("brk #1");
}

/* FAIL dont use */
void syscall(unsigned int num, unsigned int x1, unsigned int x2, unsigned int x3){
    /*
    asm volatile("mov x0, %0;"
                 : "=r"(x3));
    asm volatile("mov x1, %0;"
                 : "=r"(x2));
    asm volatile("mov x2, %0;"
                 : "=r"(x1));
    asm volatile("mov x3, %0;"
                 : "=r"(num));
                 */

    /*
    asm volatile("mov x0, %0;"
                 : "=r"(num));
    asm volatile("mov x1, %0;" ::"r"((unsigned int *)x1));
    asm volatile("mov x2, %0;" ::"r"((unsigned int *)x2));
    asm volatile("mov x3, %0;" ::"r"((unsigned int *)x3));

    asm volatile("svc #0x80");
    */
};