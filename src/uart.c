/*
 * uart.c
 * Uart stdio
 */

#include "mm.h"
#include "uart.h"

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
    while(1)
        if(mm_read(AUX_MU_LSR_REG)&0x20) 
            break;
    mm_write(AUX_MU_IO_REG,c);
}

uint8_t uart_getc()
{
    while(1)
        if(mm_read(AUX_MU_LSR_REG)&0x01) 
            break;
    return(mm_read(AUX_MU_IO_REG)&0xFF);
}

void uart_puts(const char *str)
{
    for (size_t i = 0; str[i] != '\0'; i++)
        uart_putc((uint8_t)str[i]);
}

void uart_read_line(char *buffer)
{
    size_t position = 0;
    uint8_t c;

    while(true) {
        c = uart_getc();
        uart_putc(c);

        if (c == '\r') {
            buffer[position] = '\0';
            uart_putc('\n');
            return;
        } else {
            buffer[position] = c;
        }
        position++;
    }
}


