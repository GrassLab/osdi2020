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

void test1(){
	int a = open("hello", O_CREAT);
	int b = open("world", O_CREAT);
	write(a, "Hello ", 6);
	write(b, "World!", 6);
	close(a);
	close(b);
	b = open("hello", 0);
	a = open("world", 0);

	char buf[100];	
	int sz;
	sz = read(b, buf, 100);
	sz += read(a, buf + sz, 100);
	buf[sz] = '\0';
	printf("%s\n", buf); // should be Hello World!
	close(a);
	close(b);


}

void test2(){
	int root = open("/", 0);
        if(root==-1){
		printf("ls end\r\n");
	}
}

void test3(){
	char buf[8];
	mkdir("mnt");
	mkdir("mnt/a");
        int fd = open("/mnt/a/a.txt", O_CREAT);
	write(fd, "Hi", 2);
	close(fd);
	
	int sz;
	fd = open("mnt/a/a.txt", 0);
	sz = read(fd, buf, 100);
	close(fd);
	buf[sz] = '\0';
	printf("%s\r\n",buf);

}

void main()
{
	printf("\r\n@@@ Hello for user %d @@@\r\n",get_taskid());
	
	printf("\r\n====== Test1 ======\r\n");
	test1();
	printf("\r\n====== Test2 ======\r\n");
	test2();
	printf("\r\n====== Test3 ======\r\n");
	test3();

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
