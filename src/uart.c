/*
 * uart.c
 * Uart stdio
 */

#include "mm.h"
#include "uart.h"
#include "string.h"

void uart_setup()
{
    unsigned int selector;

    selector = mm_read(GPFSEL1);
    selector &= ~(7<<12);                   // clean gpio14
    selector |= 2<<12;                      // set alt5 for gpio14
    selector &= ~(7<<15);                   // clean gpio15
    selector |= 2<<15;                      // set alt5 for gpio15
    mm_write(GPFSEL1,selector);

    mm_write(GPPUD,0);
    delay(150);
    mm_write(GPPUDCLK0,(1<<14)|(1<<15));
    delay(150);
    mm_write(GPPUDCLK0,0);

    mm_write(AUX_ENABLE,1);                   //Enable mini uart (this also enables access to it registers)
    mm_write(AUX_MU_CNTL_REG,0);               //Disable auto flow control and disable receiver and transmitter (for now)
    mm_write(AUX_MU_IER_REG,0);                //Disable receive and transmit interrupts
    mm_write(AUX_MU_LCR_REG,3);                //Enable 8 bit mode
    mm_write(AUX_MU_MCR_REG,0);                //Set RTS line to be always high
    mm_write(AUX_MU_BAUD_REG,270);             //Set baud rate to 115200

    mm_write(AUX_MU_CNTL_REG,3);               //Finally, enable transmitter and receiver
}

void uart_putc(uint8_t c)
{
    while(!(mm_read(AUX_MU_LSR_REG)&0x20));
    mm_write(AUX_MU_IO_REG,c);
}

uint8_t uart_getc() {
    while(!(mm_read(AUX_MU_LSR_REG)&0x01));
    return(mm_read(AUX_MU_IO_REG)&0xFF);
}

void uart_puts(const char *str) {
    for (size_t i = 0; str[i] != '\0'; i++)
        uart_putc((uint8_t)str[i]);
}

void uart_read_line(char *buffer, size_t size) {
    size_t position = 0;
    uint8_t c, e;

    while(position < size) {
        c = uart_getc();

        if (c == '\r' || c == '\n') {
            buffer[position++] = '\0';
            uart_puts("\r\n");
            return;
        } else if (c == 127) {
            if (position > 0) {
                buffer[--position] = 0;
                uart_puts("\b \b");
            }
        } else if (c == '[') {
            e = uart_getc();
            if (e == 'C' && position < strlen(buffer)) {
                uart_puts("\033[C");
                position++;
            } else if (e == 'D' && position > 0) {
                uart_puts("\033[D");
                position--;
            }
        } else if (c > 39 && c < 127) {
            buffer[position++] = c;
            uart_putc(c);
        }
    }
}
