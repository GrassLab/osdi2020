#include "user_lib.h"
#include "string.h"
void test_command1() { // test fork functionality
	int cnt = 0;
	if(fork() == 0) {
		fork();
		fork();
    		while(cnt < 10) {
			// address should be the same across tasks, 
			// but the cnt should be increased indepndently
			unsigned long cnt_addr = (unsigned long)&cnt;
      			printf("task id: %d, sp: 0x%x cnt: %d\n", get_taskid(), cnt_addr, cnt++);
			delay(1000000);
    		}
    		exit(0); // all childs exit
  	}
}

void test_command2() { // test page fault
	int pid = fork();
  	if(pid == 0) {
    		int* a = 0x0; // a non-mapped address.
    		printf("%d\n", *a); // trigger simple page fault, child will die here.
  	}
	else{
		printf("chiild pid: %d\r\n",pid);
	}
}

void test_command3() { // test page reclaim.
	// get number of remaining page frames from kernel by system call.
 	printf("Remaining page frames : %d\n", get_remain_page_num()); 
}

void read_beyond_boundary(){
 	if(fork() == 0) {	
    		int* ptr = mmap(NULL, 4096, PROT_READ, MAP_ANONYMOUS, NULL, 0);
		printf("addr: 0x%x\n", ptr);
		fork();
    		printf("ptr[1000]: %d\n", ptr[1000]); // should be 0
    		printf("ptr[4097]: %d\n", ptr[4097]); // should be seg fault
  	}
}

void write_beyond_boundary(){
  	if(fork() == 0) {
    		int* ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, NULL, 0);
		printf("addr: 0x%x\n", ptr);
    		if(fork()==0){
			ptr[1000]++;
   			printf("ptr[1000]: %d\n", ptr[1000]); // should be 100
    			ptr[4097] = 100;// should be seg fault
    			printf("%d\n", ptr[4097]); // not reached
		}
		else{
			ptr[1000] = 100;
   			printf("ptr[1000]: %d\n", ptr[1000]); // should be 100
    			ptr[4097] = 100;// should be seg fault
    			printf("%d\n", ptr[4097]); // not reached
		}
  	}
}

void wrong_permission(){
  	if(fork() == 0) {
    		int* ptr = mmap(NULL, 4096, PROT_READ, MAP_ANONYMOUS, NULL, 0);
    		printf("addr: %llx\n", ptr);
    		printf("ptr[1000]: %d\n", ptr[1000]); // should be 0
    		for(int i = 0; i < 4096; ++i) {
     	 		ptr[i] = i+1; // should be seg fault
   		 }
    		
		for(int i = 0; i < 4096; ++i) { // not reached
      			printf("%d\n", ptr[i]);
    		}
  	}
}


void mmaps(){ // test multiple mmaps
 	 if(fork() == 0) {
		
    		for(int i = 0; i < 16; ++i){
      			if ( i < 6 ) {
        			mmap(NULL, 4096, PROT_WRITE|PROT_READ, MAP_ANONYMOUS, NULL, 0);
      			} 
			else if(i < 12){
        			mmap(NULL, 4096, PROT_WRITE, MAP_ANONYMOUS, NULL, 0);
     			} 
			else {
        			mmap(NULL, 4096, PROT_WRITE|PROT_READ, MAP_ANONYMOUS, NULL, 0);
      			}
    		}
				
		wait(); // sleep and never wake up, just dont want scheduler schedule me
			// note that the task will never be killed, anyway.......
    		while(1); // hang to let shell see the mapped regions
 	 }
}

void mmap_unalign(){
  	if(fork() == 0) {
    		printf("0x%x", mmap((void*)0x12345678, 0x1fff, PROT_WRITE|PROT_READ, MAP_ANONYMOUS, NULL, 0)); // should be a page aligned address A and region should be A - A +0x2000
		
		wait(); // sleep and never wake up, just dont want scheduler schedule me
    		while(1); // hang to let shell see the mapped regions
  	}
}


int check_string(char * str){

	if(strcmp(str,"help")==0){
		// print all available commands
		printf("hello : print Hello World!\r\n");
		printf("help : help\r\n");
		printf("reboot: reboot raspi\r\n");
		printf("t1: test1 command\r\n");
		printf("t2: test2 command\r\n");
		printf("t3: test3 command\r\n");
		printf("\r\n");
		printf("m1: read beyond boundary\r\n");
		printf("m2: write beyond boundary\r\n");
		printf("m3: wrong permission\r\n");
		printf("m4: mmaps\r\n");
		printf("m5: mmap align\r\n");
		printf("exit : exit the user program\r\n");
	}
	else if(strcmp(str,"hello")==0){
		// print hello
		printf("Hello World!\r\n");
	}
	else if(strcmp(str,"exit")==0){
		printf("Now exit\r\n");
		return -1;
	}
	else if(strcmp(str,"reboot")==0){
		printf("Rebooting......\r\n");
		reboot();
		return 1;
	}
	else if(strcmp(str,"t1")==0){
		test_command1();
	}
	else if(strcmp(str,"t2")==0){
		test_command2();
	}
	else if(strcmp(str,"t3")==0){
		test_command3();
	}
	else if(strcmp(str,"m1")==0){
		 read_beyond_boundary();
	}
	else if(strcmp(str,"m2")==0){
		 write_beyond_boundary();
	}
	else if(strcmp(str,"m3")==0){
		 wrong_permission();
	}
	else if(strcmp(str,"m4")==0){
		 mmaps();
	}
	else if(strcmp(str,"m5")==0){
		 mmap_unalign();
	}
	else{
		printf("Err:command ");
		printf(str);
		printf(" not found, try <help>\r\n");
	}

	return 0;
}

void main()
{
	printf("Hello for user %d\r\n",get_taskid());
	
	char buffer[128];
	while(1){	
		printf(">>");
		int success = 0;
        	success = uart_read(buffer,sizeof(buffer));
		buffer[success] = '\0'; //make buffer a valid string
		int ret = check_string(buffer);
		if(ret==-1)
			return;
		else if(ret==1)
			while(1);
	}

}
