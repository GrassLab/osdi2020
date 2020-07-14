#include "include/mbox.h"
#include "include/peripherals/uart.h"
#include "include/uart.h"
#include "include/peripherals/gpio.h"
#include "include/utils.h"
#include "include/queue.h"
#include "include/scheduler.h"
#include "include/irq.h"
#include "include/mm.h"
#include "include/sprintf.h"

extern volatile unsigned char _kernel_end;

void uart_init(){
    //init uart irq
    put32(IRQ_ENABLE1,1<<25);
    
    /* initialize UART */
    put32(UART0_CR,0);         // turn off UART0

    /* set up clock for consistent divisor values */
    mbox[0] = 9*4;
    mbox[1] = REQUEST_CODE;
    mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    mbox[3] = 12;          // request length
    mbox[4] = 8;           // response length
    mbox[5] = 2;           // UART clock id
    mbox[6] = 4000000;     // rate = 4Mhz
    mbox[7] = 0;           // clear turbo
    mbox[8] = END_TAG;
    mbox_call(8);
    
    unsigned int selector;

    selector = get32(GPFSEL1);
    selector &= ~(7<<12); // clean gpio 14
    selector |= 4<<12;    // set alt0 for gpio4
    selector &= ~(7<<15); // clean gpio 15
    selector |= (4<<15);  // set alt0 for gpio5 
    put32(GPFSEL1,selector);

    put32(GPPUD,0);
    delay(150);
    put32(GPPUDCLK0,(1<<14)|(1<<15));
    delay(150);
    put32(GPPUDCLK0,0);

 
    put32(UART0_ICR,0x7FF); //clear interrupt
    
    put32(UART0_IBRD,2); // (4 × 10^6) / (16 × 115200) = 2.17
    put32(UART0_FBRD,0xB); // int((0.17 × 64) + 0.5) = 11
    put32(UART0_LCRH,0b11<<5); //word length = 8bits
    put32(UART0_CR,0x301); // enable Tx,Rx,FIFO   

    // init uart0 interrupt
    put32(UART0_IMSC,0x30); // interrupt mask

    // init buffer for handling interrupt
    read_buf_head = 0;
    read_buf_tail = 0;
    write_buf_head = 0;
    write_buf_tail = 0; 
}


int uart_send ( char c ) // If fail to send, return -1 
{	
	char tmp;

	if(get32(UART0_FR)&0x80){ //transmit FIFO empty
		if(isempty(write_buf_head,write_buf_tail))//queue empty
			put32(UART0_DR,c);
		else{
			tmp=pop(write_buf,&write_buf_head);
			push(write_buf,&write_buf_tail,c);

			put32(UART0_DR,tmp);	
		}
	}
	else{   // If FIFO is full, then put our data in queue.

		if(!isfull(write_buf_head,write_buf_tail)){
			push(write_buf,&write_buf_tail,c);
		}
		else{
		// If FIFO, also the queue if full, then char will be 
		// dropped...
			return -1;
		}
	}
	return 0;
}

char uart_recv ()
{
	while(isempty(read_buf_head,read_buf_tail)){

	}
	
	return pop(read_buf,&read_buf_head);
}

void uart_send_string(char* str)
{
	for (int i = 0; str[i] != '\0'; i ++) {
		uart_send((char)str[i]);
	}

}

void uart_recv_string(int size, char *buf){
	char recv_char;
	for(int i=0;i<size;i++){
		
		recv_char = uart_recv();
		uart_send(recv_char);
			
		if(recv_char =='\n' || recv_char == '\r'){
			buf[i] = '\0';
			break;
		}
		else{
	  		buf[i] = recv_char;
		}
	}
}

void uart_hex(unsigned int d) {
    unsigned int n;
    int c;
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        uart_send(n);
    }
}

void uart_IRQhandler(){
	unsigned int status = get32(UART0_MIS);	
	char c;
	
	if(status&0x10){ // for receive	

		while(get32(UART0_FR)&0x40){//receive FIFO full
			c = get32(UART0_DR)&0xFF;
			if(waitqueue.heap_size>0){ //If input, task in waitQ will be put back to runQ
				int target = priorityQ_pop(&waitqueue);
				struct task_struct *p = task[target];
				p->state = TASK_RUNNING;
				priorityQ_push(&runqueue,p->priority,p->pid);
			}
			if(!isfull(read_buf_head,read_buf_tail)){
				push(read_buf,&read_buf_tail,c);
			}
		}

		put32(UART0_ICR,status); //clear interrupt
	}
	else{	
		while(!isempty(write_buf_head,write_buf_tail)){
			c = pop(write_buf,&write_buf_head);

			while(get32(UART0_FR)&0x20); //transmit FIFO full
			put32(UART0_DR,c);
		}
		put32(UART0_ICR,status); //clear interrupt
	}
}

/**
 * Display a string
 */
void printf(char *fmt, ...) {
	__builtin_va_list args;
	__builtin_va_start(args, fmt);
    	// we don't have memory allocation yet, so we
    	// simply place our string after our code
   	char *s = (char*)&_kernel_end;
    
	// use sprintf to format our string
    	vsprintf(s,fmt,args);
    	
	// print out as usual
    	while(*s) {
        /* convert newline to carrige return + newline */
        	if(*s=='\n')
            		uart_send('\r');
        	uart_send(*s++);
    	}
}

/**
 * Dump memory
 */
void uart_dump(void *ptr)
{
    unsigned long a,b,d;
    unsigned char c;
    
    for(a=(unsigned long)ptr;a<(unsigned long)ptr+512;a+=16) {
        uart_hex(a); 
	uart_send_string(": ");
        for(b=0;b<16;b++) {
            c=*((unsigned char*)(a+b));
            d=(unsigned int)c;d>>=4;d&=0xF;d+=d>9?0x37:0x30;uart_send(d);
            d=(unsigned int)c;d&=0xF;d+=d>9?0x37:0x30;uart_send(d);
            uart_send(' ');
            if(b%4==3)
                uart_send(' ');
        }
        for(b=0;b<16;b++) {
            c=*((unsigned char*)(a+b));
            uart_send(c<32||c>=127?'.':c);
        }
        uart_send('\r');
        uart_send('\n');
    }
}
