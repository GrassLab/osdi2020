#include "user_lib.h"
#include "printf.h"
/*
void user_main() 
{
	printf("sys_remain_page_num: %d\r\n", sys_remain_page_num());
	printf("User process\r\man");
	int pid = call_sys_fork();
	if (pid < 0) {
		printf("Error during fork\r\n");
		call_sys_exit();
		return;
	}
	if (pid == 0){
        while (1) {
            printf("abcde\r\n");
			call_sys_exit();
        }
	} else {
        while (1) {
			printf("sys_remain_page_num: %d\r\n", sys_remain_page_num());
			printf("task id: %d\r\n", sys_get_task_id());
            printf("1234\r\n");
			user_delay(1000000000);
        }
	}
    call_sys_exit();
}*/


void user_main() {
	int cnt = 0;
	if(fork() == 0) {
		fork();
		fork();
		while(cnt < 10) {
			printf("task id: %d, sp: 0x%x cnt: %d\n", get_taskid(), &cnt, cnt++);
			delay(100000000);
		}
		exit();
	}
}