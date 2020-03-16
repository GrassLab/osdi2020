#include "include/utils.h"
#include "include/peripherals/mini_uart.h"
#include "include/peripherals/gpio.h"
#include "include/string.h"
#include "include/reboot.h"

void uart_send ( char c )
{
	while(1) {
		if(get32(AUX_MU_LSR_REG)&0x20) 
			break;
	}
	put32(AUX_MU_IO_REG,c);
}

char uart_recv ( void )
{
	while(1) {
		if(get32(AUX_MU_LSR_REG)&0x01) 
			break;
	}
	return(get32(AUX_MU_IO_REG)&0xFF);
}

void uart_send_string(char* str)
{
	for (int i = 0; str[i] != '\0'; i ++) {
		uart_send((char)str[i]);
	}

}

int uart_check_string(char * str){
	char* cmd_help = "help";
	char* cmd_hello = "hello";
	char* cmd_time = "timestamp";
	char* cmd_reboot = "reboot";

	if(strcmp(str,cmd_help)==0){
	// print all available commands
		uart_send_string("\r\nhello : print Hello World!\r\n");
		uart_send_string("help : help\r\n");
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
	else{
		uart_send_string("\r\nErr:command ");
		uart_send_string(str);
		uart_send_string(" not found, try <help>\r\n");
	}
	return 0;
}

void uart_init ( void )
{
	unsigned int selector;

	selector = get32(GPFSEL1);
	selector &= ~(7<<12);                   // clean gpio14
	selector |= 2<<12;                      // set alt5 for gpio14
	selector &= ~(7<<15);                   // clean gpio15
	selector |= 2<<15;                      // set alt5 for gpio15
	put32(GPFSEL1,selector);

	put32(GPPUD,0);
	delay(150);
	put32(GPPUDCLK0,(1<<14)|(1<<15));
	delay(150);
	put32(GPPUDCLK0,0);

	put32(AUX_ENABLES,1);                   //Enable mini uart (this also enables access to it registers)
	put32(AUX_MU_CNTL_REG,0);               //Disable auto flow control and disable receiver and transmitter (for now)
	put32(AUX_MU_IER_REG,0);                //Disable receive and transmit interrupts
	put32(AUX_MU_LCR_REG,3);                //Enable 8 bit mode
	put32(AUX_MU_MCR_REG,0);                //Set RTS line to be always high
	put32(AUX_MU_BAUD_REG,270);             //Set baud rate to 115200

	put32(AUX_MU_CNTL_REG,3);               //Finally, enable transmitter and receiver
}
