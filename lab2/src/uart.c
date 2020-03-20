#include "utils.h"
#include "peripherals/uart.h"
#include "peripherals/gpio.h"

void uart_init() {
    // Formula from BCM2837-ARM-Peripherals.-.Revised.-.V2-1.pdf
    // Page 183
    // Baud rate divisor BAUDDIV = (FUARTCLK/(16 Baud rate))
    // where FUARTCLK is the UART reference clock frequency.
    // The BAUDDIV is comprised of the integer value IBRD and the fractional value FBRD.
    // See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0183g/I54603.html
    // Basically, 
    // ibrd = (UARTCLK / (16 * UART_BAUD_RATE)) & 0xFFFF = 26
    // fbrd = (int)((((UARTCLK * 1.0f) /  (16 * UART_BAUD_RATE)) - ibrd) * 64 + 0.5) = 3
    // We don't do this because it seems like floating point and -mgeneral-regs-only
    // don't work well together.
    unsigned int ibrd = 26;
    unsigned int fbrd = 3;
    unsigned int selector;

    // The function select registers are used to define the operation of the general-purpose I/O pins
    // GPFSEL1 is a register. See ARM peripherals manual on page 91
    selector = get32(GPFSEL1);

    // Clear GPIO 14. The bits for GPIO 14 live in bits 14-12 (see page 91)
    // We use 7 because that is `111` in binary. Then, we shift 12 bits so
    // we cover bits 14 - 12. Then we negate that to make all bits ones except
    // for bits 14 - 12 (which will now be zero). Finally, we bitwise AND that
    // with the selector to make only the bits corresponding to GPIO 14 equal to 0.
    selector &= ~(7<<12);

    // Turn on the bit to turn on alternate function 0 for Pin 14 (TXD0).
    // For this, we need to set the bits to be `100`.
    selector |= 0x4 << 12;

    // Do the same for Pin 15. In this case, we're turning on RXD1 (Read instead of transmit).
    selector &= ~(7<<15);
    selector |= 0x4 << 15;

    put32(GPFSEL1, selector);


    // Set the GPIO for Pull Down. This means that, if a Pin is not connected, it will report as
    // 0 instead of a random value. We could have set this value to 1 to make it pull up instead.
    // Instructions (more info in ARM Peripherals page 101):
    // 1. Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither
    // to remove the current Pull-up/down)
    // 2. Wait 150 cycles – this provides the required set-up time for the control signal
    // 3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to
    // modify – NOTE only the pads which receive a clock will be modified, all others will
    // retain their previous state.
    // 4. Wait 150 cycles – this provides the required hold time for the control signal
    // 5. Write to GPPUD to remove the control signal (no need to do this one since we're setting this to 0).
    // 6. Write to GPPUDCLK0/1 to remove the clock
    put32(GPPUD, 0);
    delay(150);
    // We're passing a bit-map here where we set only bits 14 and 15  (representing pins 14 and 15).
    put32(GPPUDCLK0, (1<<14)|(1<<15));
    delay(150);
    put32(GPPUDCLK0, 0);


    put32(UART_CR, 0); // Disable UART
    put32(UART_IMSC, 0); // Disable interrupts
    put32(UART_IBRD, ibrd);
    put32(UART_FBRD, fbrd);
    put32(UART_FBRD, fbrd);

    // Enable 8-bit mode|FIFO
    // Page 184
    put32(UART_LCRH, (3 << 5)|(1 << 4));

    // Enable the Read|Write|UART (page 186)
    put32(UART_CR, (1<<9)|(1<<8)|1);

}

void uart_send(char c) {
    // while transmit FIFO is full (page 181)
    while (get32(UART_FR) & (1 << 5));
    put32(UART_DR, c);
}

char uart_recv() {
    // while the Receive FIFO is empty (page 181)
    while (get32(UART_FR) & (1 << 4));
    return get32(UART_DR)&0xFF;
}

void uart_send_string(char *str) {
    for (char c = *str; c != '\0'; c = *(++str)) {
        uart_send(c);
    }
}