#include "fork.h"
#include "printf.h"
#include "utils.h"
#include "sched.h"
#include "mm.h"

void sys_write(char * buf){
	printf(buf);
}

int sys_fork(){
	return copy_process(0, 0, 0);
}

void sys_exit(){
	exit_process();
}

int sys_remain_page_num() {
	return 	get_remain_num();
}

int sys_task_id() {
	return current->task_id;
}

void * const sys_call_table[] = {sys_write, sys_fork, sys_exit, sys_remain_page_num, sys_task_id};
