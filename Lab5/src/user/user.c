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
      			user_printf("task id: %d, sp: 0x%x_%x cnt: %d\n", get_taskid(), cnt_addr>>32, cnt_addr, cnt++);
			user_delay(1000000);
    		}
    		exit(0); // all childs exit
  	}
}

void test_command2() { // test page fault
	int pid = fork();
  	if(pid == 0) {
    		int* a = 0x0; // a non-mapped address.
    		user_printf("%d\n", *a); // trigger simple page fault, child will die here.
  	}
	else{
		user_printf("chiild pid: %d\r\n",pid);
	}
}

void test_command3() { // test page reclaim.
	// get number of remaining page frames from kernel by system call.
 	user_printf("Remaining page frames : %d\n", get_remain_page_num()); 
}

int check_string(char * str){
	char* cmd_help = "help";
	char* cmd_hello = "hello";
	char* cmd_exit = "exit";
	char* cmd_reboot = "reboot";
	char* cmd_test1 = "t1";
	char* cmd_test2 = "t2";
	char* cmd_test3 = "t3";

	if(strcmp(str,cmd_help)==0){
		// print all available commands
		user_printf("hello : print Hello World!\r\n");
		user_printf("help : help\r\n");
		user_printf("reboot: reboot raspi\r\n");
		user_printf("t1: test1 command\r\n");
		user_printf("t2: test2 command\r\n");
		user_printf("t3: test3 command\r\n");
		user_printf("exit : exit the user program\r\n");
	}
	else if(strcmp(str,cmd_hello)==0){
		// print hello
		user_printf("Hello World!\r\n");
	}
	else if(strcmp(str,cmd_exit)==0){
		user_printf("Now exit\r\n");
		return -1;
	}
	else if(strcmp(str,cmd_reboot)==0){
		user_printf("Rebooting......\r\n");
		reboot();
		return 1;
	}
	else if(strcmp(str,cmd_test1)==0){
		test_command1();
	}
	else if(strcmp(str,cmd_test2)==0){
		test_command2();
	}
	else if(strcmp(str,cmd_test3)==0){
		test_command3();
	}
	else{
		user_printf("Err:command ");
		user_printf(str);
		user_printf(" not found, try <help>\r\n");
	}

	return 0;
}

void main()
{
	user_printf("Hello for user %d\r\n",get_taskid());

	char str[]="I'm user\r\n";
        uart_write(str,sizeof(str));

	
	char buffer[128];
	while(1){	
		user_printf(">>");
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
