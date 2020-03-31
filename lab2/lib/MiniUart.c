#include "peripheral/MiniUart.h"
#include "peripheral/gpio.h"

#include <stddef.h>

void delay(unsigned int clock) {
    while (clock--) {
        asm volatile("nop");
    }
}

void initUART(void) {

    unsigned int selector;

    selector = *GPFSEL1;
    selector &= ~(7u << 12) | ~(7u << 15); // clean GPIO 14, 15
    selector |= 2u << 12 | 2u << 15;       // set alt5 (txd1, rxd1) for GPIO 14, 15
    *GPFSEL1 = selector;

    // check page 101 of the BCM2837 ARM Peripherals manual
    *GPPUD = 0;
    delay(150u);
    *GPPUDCLK0 = (1u << 14) | (1u << 15);
    delay(150u);
    *GPPUDCLK0 = 0u;

    *AUX_ENABLES = 1u;
    *AUX_MU_CNTL_REG = 0u;    // disable receiver and transmitter
    *AUX_MU_IER_REG = 0u;     // disable receive and transmit interrupts
    *AUX_MU_LCR_REG = 3u;     // enable 8-bit mode for data size
    *AUX_MU_MCR_REG = 0u;     // disable auto flow control
    *AUX_MU_BAUD_REG = 270u;  // set baud rate to 115200
    *AUX_MU_IIR_REG = 6u;     // clear FIFO

    *AUX_MU_CNTL_REG = 3u;    // enable rx, tx (receiver, transmitter)
}

void sendUART(char c) {
    // wait until bit 5th set to 1
    while(!(*AUX_MU_LSR_REG & 0x20)) {}
    *AUX_MU_IO_REG = c;
}

char recvUART(void) {
    // wait until bit 1st set to 1
    while(!(*AUX_MU_LSR_REG & 0x01)) {}


    return *AUX_MU_IO_REG & 0xFF;
}

void sendStringUART(const char *str) {
    while (*str) {
        sendUART(*str++);
    }
}

static char getHexChar(char c) {
    return (c > 9) ? c - 0xa + 'a' : c + '0';
}

void sendHexUART(unsigned int hex) {
    sendStringUART("0x");
    for (size_t i = 1; i <= sizeof(unsigned int); ++i) {
        char c = (hex >> ((sizeof(unsigned int) - i) * 8)) & 0xFF;

        // send upper 4-bit
        sendUART(getHexChar(c >> 4));

        // send lower 4-bit
        sendUART(getHexChar(c & 0xf));
    }
}
