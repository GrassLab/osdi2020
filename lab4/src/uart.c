#include "gpio.h"
#include "mbox.h"
#include "uart.h"
#include "mystd.h"

void mini_uart_send(unsigned int c);
void PL011_uart_send(unsigned int c);
char mini_uart_getc();
char PL011_uart_getc();
void (*uart_send)(unsigned int);
char (*uart_getc)();
int uart0_irq_enable = 0;

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void mini_uart_init()
{
    register unsigned int r;
    /* initialize UART */
    *AUX_ENABLE |=1;       // enable UART1, AUX mini uart
    *AUX_MU_CNTL = 0;
    *AUX_MU_LCR = 3;       // 8 bits
    *AUX_MU_MCR = 0;
    *AUX_MU_IER = 0;
    *AUX_MU_IIR = 0xc6;    // disable interrupts
    *AUX_MU_BAUD = 270;    // 115200 baud
    /* map UART1 to GPIO pins */
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15)); // gpio14, gpio15
    r|=(2<<12)|(2<<15);    // alt5
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;        // flush GPIO setup
    *AUX_MU_CNTL = 3;      // enable Tx, Rx

    uart_send = mini_uart_send;
    uart_getc = mini_uart_getc;
}

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void PL011_uart_init(unsigned long clock_rate)
{
    register unsigned int r;

    /* initialize UART */
    *UART0_CR = 0;         // turn off UART0

    /* set up clock for consistent divisor values */
    mbox[0] = 9*4;
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    mbox[3] = 12;
    mbox[4] = 8;
    mbox[5] = 2;           // UART clock
    mbox[6] = clock_rate;     // 4Mhz
    mbox[7] = 0;           // clear turbo
    mbox[8] = MBOX_TAG_LAST;
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
    *UART0_LCRH = 0b11<<5; // 8n1
    *UART0_CR = 0x301;     // enable Tx, Rx, FIFO

    uart_send = PL011_uart_send;
    uart_getc = PL011_uart_getc;
}

/**
 * Send a character
 */
void mini_uart_send(unsigned int c) {
    /* wait until we can send */
    do{asm volatile("nop");}while(!(*AUX_MU_LSR&0x20));
    /* write the character to the buffer */
    *AUX_MU_IO=c;
}


/**
 * Receive a character
 */
char mini_uart_getc() {
    char r;
    /* wait until something is in the buffer */
    do{asm volatile("nop");}while(!(*AUX_MU_LSR&0x01));
    /* read it and return */
    r=(char)(*AUX_MU_IO);
    /* convert carrige return to newline */
    return r=='\r'?'\n':r;
}


int queue_empty(struct uart_buf *queue){
    return queue->head == queue->tail;
}
int queue_full (struct uart_buf *queue){
    return queue->head == (queue->tail + 1) % UARTBUF_SIZE;
}
void enqueue(struct uart_buf *queue, char c){
    queue->buf[queue->tail] = c;
    queue->tail = (queue->tail + 1) % UARTBUF_SIZE;
}
char dequeue(struct uart_buf *queue){
    char head = queue->buf[queue->head];
    queue->head = (queue->head + 1) % UARTBUF_SIZE;
    return head;
}

/**
 * Send a character
 */
void PL011_uart_send(unsigned int c) {
    if(uart0_irq_enable){
        while (queue_full(&write_buf))
            asm volatile ("nop");
        enqueue(&write_buf, c);
        *UART0_IMSC = *UART0_IMSC | (1 << 5);
    }else{
        /* wait until we can send */
        do{asm volatile("nop");}while(*UART0_FR&0x20);
        /* write the character to the buffer */
        *UART0_DR=c;
    }
}

/**
 * Receive a character
 */
char PL011_uart_getc() {
    char r;
    if(uart0_irq_enable){
       *UART0_IMSC = *UART0_IMSC | (1 << 4);
        while (queue_empty(&read_buf))
            asm volatile("nop");
        r = dequeue(&read_buf);
    }else{
        /* wait until something is in the buffer */
        do{asm volatile("nop");}while(*UART0_FR&0x10);
        /* read it and return */
        r=(char)(*UART0_DR);
    }
    /* convert carrige return to newline */
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
    uart_puts("0x");
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        uart_send(n);
    }
}

void uart_gets(char *s){
    int i = 0;
    char c;
    while((c = uart_getc()) != '\n'){
        uart_send(c);
        s[i++] = c;
        // uart_send(s[0]); debug
    }
    s[i] = '\0';
    uart_send('\r');
    uart_send('\n');
}

void uart_dec(unsigned long i) {
    if(i/10)
		uart_dec(i/10);
    uart_send((i%10) + '0');
}

void uart_double(double time){
    unsigned long t1, t2;
    unsigned long max = 1000000000;
    t1 = time;
    t2 = (time-t1)*1000000000;
    uart_dec(time);
    uart_send('.');
    while(t2 < max/10){
        uart_dec(0);
        max/=10;
    }
    uart_dec(t2);
}

void enable_uart0_irq(){
    *IRQ2_EN = 1 << 25;
    uart0_irq_enable = 1;
    read_buf.head = 0;
    read_buf.tail = 0;
    write_buf.head = 0;
    write_buf.tail = 0;
}