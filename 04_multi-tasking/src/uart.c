#include "peripherals/uart.h"
#include "peripherals/gpio.h"
#include "utils.h"

void uart_send(const char c) {
    while (1) {
        if (get32(AUX_MU_LSR_REG) & 0x20)
            break;
    }
    put32(AUX_MU_IO_REG, c);
}

char uart_recv() {
    while (1) {
        if (get32(AUX_MU_LSR_REG) & 0x01)
            break;
    }
    return (get32(AUX_MU_IO_REG) & 0xFF);
}

void uart_puts(const char *str) {
    char c = str[0];
    int i = 0;
    while (c != '\0') {
        uart_send(c);
        i++;
        c = str[i];
    }
    return;
}

void uart_init(void) {
    unsigned int selector;

    selector = get32(GPFSEL1);
    selector &= ~(7 << 12); // clean gpio14
    selector |= 2 << 12;    // set alt5 for gpio14
    selector &= ~(7 << 15); // clean gpio15
    selector |= 2 << 15;    // set alt5 for gpio15
    put32(GPFSEL1, selector);

    put32(GPPUD, 0);
    delay(150);
    put32(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);
    put32(GPPUDCLK0, 0);

    put32(AUX_ENABLES,
          1); // Enable mini uart (this also enables access to it registers)
    put32(AUX_MU_CNTL_REG, 0); // Disable auto flow control and disable receiver
                               // and transmitter (for now)
    put32(AUX_MU_IER_REG, 0);    // Disable receive and transmit interrupts
    put32(AUX_MU_LCR_REG, 3);    // Enable 8 bit mode
    put32(AUX_MU_MCR_REG, 0);    // Set RTS line to be always high
    put32(AUX_MU_BAUD_REG, 270); // Set baud rate to 115200

    put32(AUX_MU_CNTL_REG, 3); // Finally, enable transmitter and receiver
}

void uart_send_hex(unsigned int d) {
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

void uart_send_ulong(unsigned long number) {
    if (number == 0) {
        uart_send('0');
        return;
    }
    const int bufsize = 64;
    char tbuf[bufsize];
    for (int i = 0; i < bufsize; ++ i) {
        tbuf[i] = 0;
    }   
    int cur = bufsize - 2;
    while (number > 0 && cur > 0) {
        tbuf[cur] = (number % 10) + '0';
        number = number / 10; 
        cur --; 
    }    
    uart_puts(tbuf + cur + 1); 
}
