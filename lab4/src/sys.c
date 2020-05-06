#include "shed.h"
#include "mm.h"

void sys_write(char * buf){
	
}

int sys_clone(unsigned long stack){
	
}

unsigned long sys_malloc(){
	
}

void sys_exit(){
	
}

void * const sys_call_table[] = {sys_write, sys_malloc, sys_clone, sys_exit};