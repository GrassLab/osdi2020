#include "uart0.h"
#include "exception.h"
#include "mbox.h"
#include "my_string.h"
#include "peripherals/gpio.h"
#include "peripherals/irq.h"
#include "peripherals/mbox.h"
#include "peripherals/uart0.h"

struct uart_queue read_buf, write_buf;

/* UART Queue */

void uart_queue_init(struct uart_queue* q, int max) {
    q->front = 0;
    q->rear = 0;
    q->max = max;
}

int uart_queue_empty(struct uart_queue* q) {
    return q->front == q->rear;
}

int uart_queue_full(struct uart_queue* q) {
    return q->front == (q->rear + 1) % q->max;
}

void uart_queue_push(struct uart_queue* q, char val) {
    if (uart_queue_full(q)) return;  // drop if full
    q->buf[q->rear] = val;
    q->rear = (q->rear + 1) % q->max;
}

char uart_queue_pop(struct uart_queue* q) {
    if (uart_queue_empty(q)) return '\0';
    char elmt = q->buf[q->front];
    q->front = (q->front + 1) % q->max;
    return elmt;
}

/* ---- */

void uart0_init() {
    *UART0_CR = 0;  // turn off UART0

    /* Configure UART0 Clock Frequency */
    unsigned int __attribute__((aligned(16))) mbox[9];
    mbox[0] = 9 * 4;
    mbox[1] = MBOX_CODE_BUF_REQ;
    // tags begin
    mbox[2] = MBOX_TAG_SET_CLOCK_RATE;
    mbox[3] = 12;
    mbox[4] = MBOX_CODE_TAG_REQ;
    mbox[5] = 2;        // UART clock
    mbox[6] = 4000000;  // 4MHz
    mbox[7] = 0;        // clear turbo
    mbox[8] = 0x0;      // end tag
    // tags end
    mbox_call(mbox, 8);

    /* Map UART to GPIO Pins */
    // 1. Change GPIO 14, 15 to alternate function
    register unsigned int r = *GPFSEL1;
    r &= ~((7 << 12) | (7 << 15));  // Reset GPIO 14, 15
    r |= (4 << 12) | (4 << 15);     // Set ALT0
    *GPFSEL1 = r;
    // 2. Disable GPIO pull up/down (Because these GPIO pins use alternate functions, not basic input-output)
    // Set control signal to disable
    *GPPUD = 0;
    // Wait 150 cycles
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
    // Clock the control signal into the GPIO pads
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    // Wait 150 cycles
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
    // Remove the clock
    *GPPUDCLK0 = 0;

    /* Configure UART0 */
    *UART0_IBRD = 0x2;        // Set 115200 Baud
    *UART0_FBRD = 0xB;        // Set 115200 Baud
    *UART0_LCRH = 0b11 << 5;  // Set word length to 8-bits
    *UART0_ICR = 0x7FF;       // Clear Interrupts

    /* Setup Interrupt */
    *UART0_IMSC = 0b11 << 4;   // Enable Tx, Rx Interrupt
    *IRQ_ENABLE_2 |= 1 << 25;  // Enable UART Interrupt

    /* Enable UART */
    *UART0_CR = 0x301;

    uart_queue_init(&read_buf, UART0_BUF_MAX_SIZE);
    uart_queue_init(&write_buf, UART0_BUF_MAX_SIZE);
}

char uart0_read() {
    while (uart_queue_empty(&read_buf)) {
        asm volatile ("nop");
    }
    char r = uart_queue_pop(&read_buf);
    return r == '\r' ? '\n' : r;
}

void uart0_write(char c) {
    if (*UART0_FR & 0x80) { // TX FIFO Empty
        // trigger interrupt by sending one character
        if (uart_queue_empty(&write_buf)) {
            *UART0_DR = c;
        }
        else {
            uart_queue_push(&write_buf, c);
            *UART0_DR = uart_queue_pop(&write_buf);
        }
    }
    else {
        uart_queue_push(&write_buf, c); // push to write queue, drop if buffer full
    }
}

void uart_printf(char* fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    char str[1024];
    my_vsprintf(str, fmt, args);

    char* s = &str[0];
    while (*s) {
        if (*s == '\n') uart0_write('\r');
        uart0_write(*s++);
    }
}

void uart0_flush() {
    while (!(*UART0_FR & 0x10)) {
        (void)*UART0_DR;  // unused variable
    }
}