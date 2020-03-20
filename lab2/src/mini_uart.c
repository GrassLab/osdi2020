#include "utils.h"
#include "peripherals/mini_uart.h"
#include "peripherals/gpio.h"


void mini_uart_init(unsigned int baudrate) {
    // Formula from BCM2837-ARM-Peripherals.-.Revised.-.V2-1.pdf
    // page 11. Note that you need to do some algebra to reach this equation.
    // The 0xFFFF is needed because the size of this register is 16 bits (page 8).
    unsigned int baudrate_reg = (SYSTEM_CLOCK_FREQ / (8 * baudrate) - 1 ) & 0xFFFF;
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

    // Turn on the bit to turn on alternate function 5 for Pin 14 (TXD1).
    // Fot this, we need to set the bits to be `010`.
    selector |= 0x2 << 12;

    // Do the same for Pin 15. In this case, we're turning on RXD1 (Read instead of transmit).
    selector &= ~(7<<15);
    selector |= 0x2 << 15;

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

    // MU here stands for Mini Uart
    put32(AUX_ENABLES, 1); // Enable mini-uart, which gives us access to its registers.
    put32(AUX_MU_CNTL_REG, 0); // Disable transimitter/receiver until we set everything up.
    put32(AUX_MU_IER_REG, 0); // Disable interrupts (we currently can't handle interrupts).
    put32(AUX_MU_LCR_REG, 3); // Enable 8-bit mode. 7-bit is also supported.
    put32(AUX_MU_MCR_REG, 0); // Sets "RTS" line high. We don't use this. Not sure why we need to set it High.
    put32(AUX_MU_BAUD_REG, baudrate_reg);
    put32(AUX_MU_CNTL_REG, 3); // Renable transmitter/receiver now that everything is setup.

}

void mini_uart_send(char c) {
    // AUX_MU_LSR_REG in the 6th bit has 0 if the transmitter is idle. Here, we wait until it is idle
    // before we read the value.
    while (!(get32(AUX_MU_LSR_REG)&0x20))
        ;
    put32(AUX_MU_IO_REG, c);
}

char mini_uart_recv() {
    // AUX_MU_LSR_REG in the 1st bit has 1 the receive FIFO has at least 1 symbol
    while (get32(AUX_MU_LSR_REG)&0x01)
        ;
    return get32(AUX_MU_IO_REG)&0xFF;
}

void mini_uart_send_string(char *str) {
    for (char c = *str; c != '\0'; c = *(++str)) {
        mini_uart_send(c);
    }
}