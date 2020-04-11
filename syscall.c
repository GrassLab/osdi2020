#include "syscall.h"

void svc(int num){
    //asm volatile("svc %0" :"=r"(num));
    asm volatile("svc #1");
}

void brk(int num){
    //asm volatile("svc %0" :"=r"(num));
    asm volatile("brk #1");
}