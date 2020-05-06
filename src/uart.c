#include "gpio.h"
#include "io.h"
#include "task.h"
#include "uart.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init() {
    register unsigned int r;

    /* initialize UART */
    *AUX_ENABLE |= 1;  // enable UART1, AUX mini uart
    *AUX_MU_CNTL = 0;
    *AUX_MU_IER = 0;
    *AUX_MU_IIR = 0xc6;  // disable interrupts
    /* map UART1 to GPIO pins */
    r = *GPFSEL1;
    r &= ~((7 << 12) | (7 << 15));  // gpio14, gpio15
    r |= (2 << 12) | (2 << 15);     // alt5
    *GPFSEL1 = r;
    *GPPUD = 0;  // enable pins 14 and 15
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = 0;  // flush GPIO setup

    *AUX_MU_IER = IER_REG_VALUE;  // enable interrupt

    *AUX_MU_LCR = 3;  // 8 bits
    *AUX_MU_MCR = 0;
    *AUX_MU_BAUD = 270;  // 115200 baud
    *AUX_MU_CNTL = 3;    // enable Tx, Rx
}

/**
 * Send a character
 */
void uart_send(unsigned int c) {
    /* wait until we can send */
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x20));
    /* write the character to the buffer */
    *AUX_MU_IO = c;
}

/**
 * Receive a character
 */
char uart_getc() {
    struct task_t* task = get_current();
    uint64_t sp;
    task->status = WAIT;
    task->uart_elr = task->elr;
    uint64_t old_elr;
    asm volatile("mrs %0, elr_el1" : "=r"(old_elr));
    asm volatile("ldr x6, =wait_return");
    asm volatile("mov %0, x6" : "=r"(task->elr));
    queue_push(&waitqueue, task);

    asm("msr DAIFClr, 0xf");

    asm volatile("mov %0, sp" : "=r"(sp));
    task->uart_sp = sp;

    asm volatile("wait_return0:");
    schedule();
    asm volatile("wait_return:");
    task = get_current();
    uint64_t new_sp = task->uart_sp;
    asm volatile("mov x6, %0" : "=r"(new_sp));
    asm volatile("mov sp, x6");
    task->elr = task->uart_elr;
    uint64_t new_elr = old_elr;
    asm volatile("msr elr_el1, %0" : "=r"(new_elr));

    char r = buffer[buffer_read++ % BUFFER_MAX];
    asm volatile("v:");

    return r == '\r' ? '\n' : r;
}

/**
 * Display a string
 */
void uart_puts(const char* s) {
    while (*s) {
        /* convert newline to carrige return + newline */
        if (*s == '\n') uart_send('\r');
        uart_send(*s++);
    }
}

char uart_getb() {
    /* wait until something is in the buffer */
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x01));
    /* read it and return */
    char r = buffer[buffer_read++ % BUFFER_MAX];
    return r == '\r' ? '\n' : r;
}

/**
 * Display a binary value in hexadecimal
 */
void uart_hex(unsigned int d) {
    unsigned int n;
    int c;
    for (c = 28; c >= 0; c -= 4) {
        // get highest tetrad
        n = (d >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += n > 9 ? 0x37 : 0x30;
        uart_send(n);
    }
}
