#include "uart.h"
#include "utils.h"
#include "hard_info.h"

char buff[50];

void main(){
	uart_init();
	init();
	hard_info();
	char c;
	int count=0;
	int i;
	int size;
	char *kernel_adr=(char*)0x40000;
	while(1){
		c=uart_read();
		if(c==0x7f||c==0x08){
			if(count!=0){
				buff[count]='\0';
				count--;
				uart_write(0x08);
			}
		}
		else if(c==10||c==13){
			uart_write('\n');
			uart_write('\r');
			if(strcmp(buff,"help")){
				print("hello : print Hello World!\n\r");
			}
			else if(strcmp(buff,"hello")){
				print("Hello World!\n\r");
			}
			/*
			else if(strcmp(buff,"loadimg")){
				print("rec\n\r");
				size=(int)uart_read();
				size|=uart_read()<<8;
				size|=uart_read()<<16;
				size|=uart_read()<<24;
				print("OK\n");
				while(size>0){
					*(kernel_adr++)=uart_read();
					size--;
				}
				asm volatile (
        		"mov x0, x10;"
        		"mov x1, x11;"
		        "mov x2, x12;"
        		"mov x3, x13;"
        		"mov x30, 0x40000; ret"
    			);
				print("done\n");
			}
			*/
			else if(strcmp(buff,"exc")){
				asm("svc 1");
			}
			else if(strcmp(buff,"irq")){
				core_timer_enable();
			}
			else print("Err, command not found, try <help>.\n\r");
			for(i=0;i<50;i++)	buff[i]='\0';
			count=0;
			uart_write('#');
		}
		else{
			buff[count]=c;
			count++;
			uart_write(c);
		}
	}
}

