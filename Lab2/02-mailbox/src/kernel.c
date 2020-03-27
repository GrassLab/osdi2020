#include "include/uart.h"
#include "include/utils.h"
#include "include/reboot.h"
#include "include/string.h"
#include "include/mbox.h"
#include "include/framebuffer.h"

int check_string(char * str){
	char* cmd_help = "help";
	char* cmd_hello = "hello";
	char* cmd_time = "timestamp";
	char* cmd_reboot = "reboot";
	char* cmd_loadimg = "loadimg";

	if(strcmp(str,cmd_help)==0){
	// print all available commands
		uart_send_string("\r\nhello : print Hello World!\r\n");
		uart_send_string("help : help\r\n");
		uart_send_string("timestamp: get current timestamp\r\n");
		uart_send_string("reboot: reboot rpi3\r\n");
	}
	else if(strcmp(str,cmd_hello)==0){
	// print hello
		uart_send_string("\r\nHello World!\r\n");
	}
	else if(strcmp(str,cmd_time)==0){
		unsigned long long freq = get_timer_freq();
		unsigned long long counts = get_timer_counts();
		unsigned long long timestamp = counts/freq;
	        	
		float temp = (float)(counts%freq)/freq;
		temp*=10000; //scale to int place
		int timestamp_fraction = temp + .5f; //add 0.5 to cause rounding		
	        uart_send_string("\r\n[");
		
		char timestamp_buffer[128];
		itos(timestamp,timestamp_buffer,10);
		uart_send_string(timestamp_buffer);
		uart_send_string(".");
		
		char timestamp_fraction_buffer[128];
	
		if(timestamp_fraction<1000)
			uart_send_string("0");
		if(timestamp_fraction<100)
			uart_send_string("0");
		if(timestamp_fraction<10)
			uart_send_string("0");
		
		itos(timestamp_fraction,timestamp_fraction_buffer,10);
		uart_send_string(timestamp_fraction_buffer);
	
		uart_send_string("]\r\n");
	}
	else if(strcmp(str,cmd_reboot)==0){
		uart_send_string("\r\nBooting......\r\n");
		reset(10000);
		return 1;	
	}
	else if(strcmp(str,cmd_loadimg)==0){
	
	}
	else{
		uart_send_string("\r\nErr:command ");
		uart_send_string(str);
		uart_send_string(" not found, try <help>\r\n");
	}
	return 0;
}

void get_board_revision_info(){
  mbox[0] = 7 * 4; // buffer size in bytes
  mbox[1] = REQUEST_CODE;
  // tags begin
  mbox[2] = GET_BOARD_REVISION; // tag identifier
  mbox[3] = 4; // response value buffer's length.
  mbox[4] = 0; // request value buffer's length
  mbox[5] = 0; // value buffer
  // tags end
  mbox[6] = END_TAG;

  if(mbox_call(8)){
	 uart_send_string("Board Revision:"); 
 	 uart_hex(mbox[5]); // it should be 0xa020d3 for rpi3 b+
  	 uart_send_string("\r\n");
  }
  else{
  	uart_send_string("Unable to query\r\n");
  }
}


void get_VC_core_base_addr(){
  mbox[0] = 7 * 4; // buffer size in bytes
  mbox[1] = REQUEST_CODE;
  // tags begin
  mbox[2] = GET_VC_MEMORY; // tag identifier
  mbox[3] = 8; // response value buffer's length.
  mbox[4] = 0; // request value buffer's length
  mbox[5] = 0; // value buffer
  // tags end
  mbox[6] = END_TAG;

  if(mbox_call(8)){
	 uart_send_string("VC core base address:"); 
	 uart_hex(mbox[5]); //base addess in bytes
	 uart_send_string("\r\n");
  }
  else{
  	uart_send_string("Unable to query\n");
  }
}

void kernel_main(void)
{	
    uart_init();
    fb_init();
    fb_show();
    
    uart_send_string("Hello, world!\r\n");
    

    //get hardware information by mailbox
    //get_board_revision_info();
    //get_VC_core_base_addr();

    // simple shell implement
    char str[128];
    char i=0;
    int check = 0;
    char recv_char;

    *(str) = '\0';
    uart_send_string(">>");
    
    while (1) {
	recv_char = uart_recv();
    	uart_send(recv_char);
	
	if(recv_char>32 && recv_char<127){
		*(str+i)=recv_char;
		i++;
	}

	else if  (recv_char =='\n' || recv_char == '\r'){
		*(str+i) = '\0';		
		check = check_string(str); 
		
		if (check ==0){	
			i=0;
			*(str) = '\0'; //reset what it will output
			uart_send_string(">>");
		}	

	}			
	 
    }
}
