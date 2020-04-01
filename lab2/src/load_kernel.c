#include "uart.h"
#include "gpio.h"
#include "mbox.h"
#include "tools.h"
#include "get_information.h"

unsigned long long int read_new_kernel_size(){
	
	unsigned long long int hex_power[8];
	char input_buffer[1000];
	//int input_buffer_size = 0;
	int buffer_length = 0;
	int kernel_size = 0;
	int iter = 0;
	int tmp_num = 0;
	uart_puts("Please input the size of the kernel(in hex format): ");

	//input_buffer_size = uart_get_string_with_echo(input_buffer);
	uart_get_string(input_buffer);
	//kernel_size = atoulli(input_buffer);
	
	// create the hex power table
	hex_power[0] = 1;	
	for(int i=1; i<8; i++){
		hex_power[i] = hex_power[i-1] * 16;
	}
 
	// hex to dec
	iter = buffer_length = strlen(input_buffer);
	iter--;
	while(input_buffer[iter] != 'x'){
		tmp_num = (int)hexChar2int(input_buffer[iter]);
		kernel_size = kernel_size + (unsigned long long int)tmp_num * hex_power[buffer_length - iter - 1];
		iter--;
	}

	return kernel_size;
}
 
unsigned long long int read_new_kernel_address(){
	unsigned long long int hex_power[8];
	unsigned long long int kernel_address = 0;
	char input_buffer[1000];
	//int input_buffer_size = 0;
	int buffer_length = 0;
	int iter = 0;
	int tmp_num = 0;
	uart_puts("Please input the address you want to load the kernel at(in hex format): ");
	//input_buffer_size = uart_get_string_with_echo(input_buffer);
	uart_get_string(input_buffer);
	
	// create the hex power table
	hex_power[0] = 1;	
	for(int i=1; i<8; i++){
		hex_power[i] = hex_power[i-1] * 16;
	}
 
	// hex to dec
	iter = buffer_length = strlen(input_buffer);
	iter--;
	while(input_buffer[iter] != 'x'){
		tmp_num = (int)hexChar2int(input_buffer[iter]);
		kernel_address = kernel_address + (unsigned long long int)tmp_num * hex_power[buffer_length - iter - 1];
		iter--;
	}

	return kernel_address;
}
