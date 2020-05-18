#include "user_lib.h"
#include "printf.h"

void test_command1() { // test fork functionality
  int cnt = 0;
  if(fork() == 0) {
    fork();
    fork();
    while(cnt < 10) {
      printf("task id: %d, sp: 0x%x cnt: %d\r\n", get_taskid(), &cnt, cnt++); // address should be the same across tasks, but the cnt should be increased indepndently
      delay(1000000);
    }
    exit(); // all childs exit
  }
}

void test_command2() { // test page fault
  if(fork() == 0) {
    int* a = 0x0; // a non-mapped address.
    printf("%d\r\n", *a); // trigger simple page fault, child will die here.
  }
}

void test_command3() { // test page reclaim.
  printf("Remaining page frames : %d\r\n", remain_page_num()); // get number of remaining page frames from kernel by system call.
}

// ---------- elective --------------------
void read_beyond_boundary(){
  if(fork() == 0) {
    int* ptr = (int* )mmap(0, 4096, PROT_READ, 0, -1, 0);
    printf("addr: 0x%x\r\n", ptr);
    printf("%d\n", ptr[1000]); // should be 0
    printf("%d\n", ptr[4097]); // should be seg fault
  }
}

void write_beyond_boundary(){
  if(fork() == 0) {
    int* ptr = mmap(0, 4096, PROT_READ_WRITE, 0, -1, 0);
    printf("addr: 0x%x\r\n", ptr);
    ptr[1000] = 100;
    printf("%d\r\n", ptr[1000]); // should be 100
    ptr[4097] = 100;// should be seg fault
    printf("%d\r\n", ptr[4097]); // not reached
  }
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
			if (strcmp(cmd_buffer, "test4") == 0) {
				read_beyond_boundary();
			}
			if (strcmp(cmd_buffer, "test5") == 0) {
				write_beyond_boundary();
			}
			cmd_ptr = cmd_buffer;
			parse_flag = 0;
		}
		else {
			cmd_ptr++;
		}
	}
}