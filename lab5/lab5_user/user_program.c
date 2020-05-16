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

void test_command1() {
	int cnt = 0;
	if(fork() == 0) {
		while(cnt < 10) {
			printf("task id: %d, sp: 0x%x cnt: %d\n", get_taskid(), &cnt, cnt++);
			delay(100000000);
		}
		exit();
	}
}

void test_command2() {
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


void test_command3() { // test page reclaim.
  printf("Remaining page frames : %d\n", remain_page_num()); // get number of remaining page frames from kernel by system call.
}

//read char *buff , size
void user_main() {
	char cmd_buffer[100];
	char tmp[2];
	char *cmd_ptr = cmd_buffer;
	int parse_flag = 0;
	while(1) {
		read((unsigned long)cmd_ptr, 1);
		if (*cmd_ptr == '\r') {
			write("\r\n");
			parse_flag = 1;
		}
		else {
			tmp[0] = *cmd_ptr;
			tmp[1] = '\0'; 
			write((unsigned long)tmp);
		}
		if (parse_flag == 1) {
			*cmd_ptr = '\0';
			if (strcmp(cmd_buffer, "test1") == 0) {
				test_command1();
			}
			if (strcmp(cmd_buffer, "test2") == 0) {
				test_command2();
			}
			if (strcmp(cmd_buffer, "test3") == 0) {
				test_command3();
			}
			cmd_ptr = cmd_buffer;
			parse_flag = 0;
		}
		else {
			cmd_ptr++;
		}
	}
}