#include "include/uart.h"
#include "include/utils.h"


void kernel_main(void){	 
    int size=0;
    char keyword;
    //char *kernel=(char*)0x81000;

    uart_init();   

again:    
    keyword = uart_getc();
    
    unsigned long address = 0;
    address=uart_getc();
    address|=uart_getc()<<8;
    address|=uart_getc()<<16;
    address|=uart_getc()<<24;
    
    uart_hex(address);

    while(keyword!='c'){
	 uart_send(keyword);
    	 uart_send('E');
	 uart_send('R');
	 uart_send('R');
	 uart_send('\r');
	 uart_send('\n');
	 keyword = uart_getc();
    } 

    uart_send(3);
    uart_send(3);
    uart_send(3);

    // read the kernel's size
    size=uart_getc();
    size|=uart_getc()<<8;
    size|=uart_getc()<<16;
    size|=uart_getc()<<24;

    char *kernel = (char*)address;

    // Should we chekck if kernel is too large...? 
    if(size<64){ 
    	uart_send('S');
	uart_send('E');
	goto again;
    }

    uart_send('O');
    uart_send('K');

    // read the kernel
    while(size--) {
	    *kernel++ = uart_getc();
    }

    /*
    //Dump Memory
    kernel=(char*)0x81000;
    while(size_2 > 0){
          uart_hex(*kernel++);
          size_2--;
    }
    */


    // restore arguments and jump to the new kernel.
    asm volatile (
        "mov x0, x10;"
        "mov x1, x11;"
        "mov x2, x12;"
        "mov x3, x13;"
        // we must force an absolute address to branch to
        	
    );

    set_address(address);

    asm volatile(
    	"ret;"
    );
}
