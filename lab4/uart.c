#include "uart.h"
#include "mailbox.h"

/* PL011 UART registers */

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    register unsigned int r;
    

    /* initialize UART */
    *UART0_CR = 0;         // turn off UART0
    /* set up clock for consistent divisor values */
    _mbox[0] = 9*4;
    _mbox[1] = MBOX_REQUEST;
    _mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    _mbox[3] = 12;
    _mbox[4] = 8;
    _mbox[5] = 2;           // UART clock
    _mbox[6] = 4000000;     // 4Mhz
    _mbox[7] = 0;           // clear turbo
    _mbox[8] = MBOX_TAG_LAST;
    mbox_call(MBOX_CH_PROP);

    /* map UART0 to GPIO pins */
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15)); // gpio14, gpio15
    r|=(4<<12)|(4<<15);    // alt0
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;        // flush GPIO setup

    *UART0_ICR = 0x7FF;    // clear interrupts
    *UART0_IBRD = 2;       // 115200 baud
    *UART0_FBRD = 0xB;
    *UART0_LCRH = 6<<4; // 8n1 Enable FIFO, now is not enable.
    *UART0_CR = 0x301;     // enable Tx, Rx, FIFO

    //*UART0_IMSC = 3 << 4; //bit 4,5 enable mask interrupt of Tx Rx
    //*ENABLE_IRQ2 = 1 << 25; //enable uart irq

    //*UART0_IFLS = 0x9; //UART 16*8 transmit and 16*12 recive -> here we set 1/2 full for both Transmit and Recive p.175 187
    rec_buf.head=0;
    rec_buf.tail=0;
    tran_buf.head=0;
    tran_buf.tail=0;
}

/**
 * Send a character
 */
void uart_send(unsigned int c) {
    /*if(*UART0_FR&0x20)
    {
        //tran_buf.tail++;
        tran_buf.buf[tran_buf.tail++] = c;
    }
    else
    {
        *UART0_DR=c;
    }*/

    /*-----------------old version for test-----------------*/
    do{asm volatile("nop");}while(*UART0_FR&0x20);
    *UART0_DR=c;

}
 
/**
 * Receive a character
 */
char uart_getc() {
    char r;

    /*while( !(rec_buf.tail - rec_buf.head) )
        asm volatile("wfi");
    if( (rec_buf.tail - rec_buf.head) > 0)
    {
        r = rec_buf.buf[rec_buf.head++];
        if((rec_buf.tail - rec_buf.head) == 0)
        {
            tran_buf.tail = 0;
            tran_buf.head = 0;
        }
        return r == '\r' ? '\n' : r;
    }
    else
    {
        return 0;        
    }*/

    //while(1){asm volatile("nop");}

    /*-----------------old version for test-----------------*/
    
    do{asm volatile("nop");}while(*UART0_FR&0x10);
    r=(char)(*UART0_DR);
    return r=='\r'?'\n':r;

}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while(*s) {
        /* convert newline to carrige return + newline */
        if(*s=='\n')
            uart_send('\r');
        uart_send(*s++);
    }
}

/**
 * Display a binary value in hexadecimal
 */
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