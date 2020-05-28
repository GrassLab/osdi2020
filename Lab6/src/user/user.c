#include "user_lib.h"
#include "string.h"

int check_string(char * str){

	if(strcmp(str,"help")==0){
		// print all available commands
		printf("hello : print Hello World!\r\n");
		printf("help : help\r\n");
		printf("reboot: reboot raspi\r\n");
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
	int *m = (int *)malloc(sizeof(int)*10);	
	m[12] = 5;
	printf("m[12] = %d at 0x%x\r\n",m[12],m);
	free((unsigned long)m);
	
	//`m = (int *)malloc(sizeof(int)*10);	
	m[12] = 5;
	//printf("m[12] = %d at 0x%x\r\n",m[12],m);
	
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
