#include "uart.h"
#include "gpio.h"
#include "mbox.h"
#include "tools.h"
#include "get_information.h"

void load_kernel_at_target_address(const char * target_address){



} 
unsigned long long int read_kernel_address(){
	unsigned long long int hex_power[8];
	unsigned long long int kernel_address = 0;
	char input_address_buffer[1000];
	int buffer_length = 0;
	int iter = 0;
	int tmp_num = 0;

	// create the hex power table
	hex_power[0] = 1;	
	for(int i=1; i<8; i++){
		hex_power[i] = hex_power[i-1] * 16;
	}
 
	// read the kernel address 
	iter = buffer_length = strlen(input_address_buffer);
	iter--;
	while(input_address_buffer[iter] != 'x'){
		tmp_num = hexChar2int(input_address_buffer[iter]);
		kernel_address = kernel_address + (unsigned long long int)tmp_num * hex_power[buffer_length - iter - 1];
		iter--;
	}
	return kernel_address;
}
