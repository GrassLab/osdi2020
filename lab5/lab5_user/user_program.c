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
    int* ptr = (int* )mmap(0, 4096, PROT_READ, MAP_ANONYMOUS, -1, 0);
    printf("addr: 0x%x\r\n", ptr);
    printf("%d\n", ptr[1000]); // should be 0
    printf("%d\n", ptr[4097]); // should be seg fault
  }
}

void write_beyond_boundary(){
  if(fork() == 0) {
    int* ptr = mmap(0, 4096, PROT_READ_WRITE, MAP_ANONYMOUS, -1, 0);
    printf("addr: 0x%x\r\n", ptr);
    ptr[1000] = 100;
    printf("%d\r\n", ptr[1000]); // should be 100
    ptr[4097] = 100;// should be seg fault
    printf("%d\r\n", ptr[4097]); // not reached
  }
}

void wrong_permission(){
  if(fork() == 0) {
    int* ptr = mmap(0, 4096, PROT_READ, MAP_ANONYMOUS, -1, 0);
    printf("addr: 0x%x\r\n", ptr);
    printf("%d\r\n", ptr[1000]); // should be 0
    for(int i = 0; i < 4096; ++i) {
      ptr[i] = i+1; // should be seg fault
    }
    for(int i = 0; i < 4096; ++i) { // not reached
      printf("%d\r\n", ptr[i]);
    }
  }
}



void mmaps(){ // test multiple mmaps
  if(fork() == 0) {
    for(int i = 0; i < 40; ++i){
      if ( i < 20 ) {
        mmap(NULL, 4096, PROT_READ_WRITE, MAP_ANONYMOUS, -1, 0);
      } else if(i < 30){
        mmap(NULL, 4096, PROT_READ_WRITE, MAP_ANONYMOUS, -1, 0);
      } else {
        mmap(NULL, 4096, PROT_READ_WRITE, MAP_ANONYMOUS, -1, 0);
      }
    }
    while(1); // hang to let shell see the mapped regions
  }
}

void mmap_unalign(){
  if(fork() == 0) {
    printf("0x%x", mmap(0x12345678, 0x1fff, PROT_READ_WRITE, MAP_ANONYMOUS, -1, 0)); // should be a page aligned address A and region should be A - A +0x2000
    while(1); // hang to let shell see the mapped regions
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
			else if (strcmp(cmd_buffer, "test2") == 0) {
				test_command2();
			}
			else if (strcmp(cmd_buffer, "test3") == 0) {
				test_command3();
			}
			else if (strcmp(cmd_buffer, "test4") == 0) {
				read_beyond_boundary();
			}
			else if (strcmp(cmd_buffer, "test5") == 0) {
				write_beyond_boundary();
			}
			else if (strcmp(cmd_buffer, "test6") == 0) {
				wrong_permission();
			}
			else if (strcmp(cmd_buffer, "test7") == 0) {
				mmaps();
			}
			else if (strcmp(cmd_buffer, "test8") == 0) {
				mmap_unalign();
			}
			cmd_ptr = cmd_buffer;
			parse_flag = 0;
		}
		else {
			cmd_ptr++;
		}
	}
}