#include "user_lib.h"
#include "string.h"

int check_string(char * str){
	char* cmd_help = "help";
	char* cmd_hello = "hello";
	char* cmd_exit = "exit";

	if(strcmp(str,cmd_help)==0){
		// print all available commands
		user_printf("hello : print Hello World!\r\n");
		user_printf("help : help\r\n");
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
		if(check_string(buffer)==-1)
			return;
	}

}
