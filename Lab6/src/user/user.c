#include "user_lib.h"
#include "string.h"

void test1(){
	int *m = (int *)malloc(0x4000);	
	m[0] = 5;
	printf("m[0] = %d at 0x%x\r\n",m[0],m);
	m[3200] = 1;
	printf("m[3200] = %d at 0x%x\r\n",m[3200],m+3200);
	free((unsigned long)m);
	
	m = (int *)malloc(0x4000);	
	printf("at 0x%x\r\n",m);
	free((unsigned long)m);
	
	m = (int *)malloc(0x4000);		
	printf("at 0x%x\r\n",m);
	free((unsigned long)m);
}

void test2(){
	int pool_num = allocator_register(0x100);
	printf("### pool num %d\r\n",pool_num);
	unsigned long test_ptr1;
	unsigned long test_ptr2;
 
	// allocate memory from memory pool 
	test_ptr1 = allocator_alloc(pool_num);
	test_ptr2 = allocator_alloc(pool_num);
	printf("the return address 0x%x\r\n",test_ptr1);
	printf("the return address 0x%x\r\n",test_ptr2);

	allocator_free(pool_num,test_ptr1);
	test_ptr1 = allocator_alloc(pool_num);
	printf("the return address 0x%x\r\n",test_ptr1);

   	unsigned long test_ptr3;
	test_ptr3 = allocator_alloc(pool_num);
	printf("the return address 0x%x\r\n",test_ptr3);

	allocator_unregister(pool_num);	
}

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
	else if(strcmp(str,"t1")==0){
		test1();
	}
	else if(strcmp(str,"t2")==0){
		test2();
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
