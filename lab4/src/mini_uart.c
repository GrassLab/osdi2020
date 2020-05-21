#include "utils.h"
#include "peripherals/mini_uart.h"
#include "peripherals/gpio.h"
//#define REQ_1_2
#define REQ_3_4

void reserve(char *str,int index)
{
    int i = 0, j = index - 1, temp;
    while (i < j) {
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp;
        i++;
        j--;
    }
}

void uart_send ( char c )
{
	while(1) {
		if(get32(AUX_MU_LSR_REG)&0x20) 
			break;
	}
	put32(AUX_MU_IO_REG,c);
}

char uart_recv ( void )
{
	while(1) {
		if(get32(AUX_MU_LSR_REG)&0x01) 
			break;
	}
	return(get32(AUX_MU_IO_REG)&0xFF);
}


void uart_send_string(char* str)
{   
    char c = str[0];
    int  i = 0;
    while (c != '\0') {
        uart_send(c);
        i++;
        c = str[i];
    }
    return;
}

void uart_init ( void )
{
	unsigned int selector;

	selector = get32(GPFSEL1);
	selector &= ~(7<<12);                   // clean gpio14
	selector |= 2<<12;                      // set alt5 for gpio14
	selector &= ~(7<<15);                   // clean gpio15
	selector |= 2<<15;                      // set alt5 for gpio15
	put32(GPFSEL1,selector);

	put32(GPPUD,0);
	delay(150);
	put32(GPPUDCLK0,(1<<14)|(1<<15));
	delay(150);
	put32(GPPUDCLK0,0);

	put32(AUX_ENABLES, 1);                   //Enable mini uart (this also enables access to it registers)
	put32(AUX_MU_CNTL_REG, 0);               //Disable auto flow control and disable receiver and transmitter (for now)
	put32(AUX_MU_IER_REG, 1);                //Disable receive and transmit interrupts
	put32(AUX_MU_LCR_REG, 3);                //Enable 8 bit mode
	put32(AUX_MU_MCR_REG, 0);                //Set RTS line to be always high
	put32(AUX_MU_BAUD_REG, 270);             //Set baud rate to 115200

	put32(AUX_MU_CNTL_REG,3);               //Finally, enable transmitter and receiver
}

void _putc (char c)
{
    char temp = c;
    sync_call_uart_write(&temp,1);
    return;
}

// This function is required by printf function
void putc ( void* p, char c)
{
    #ifdef REQ_1_2 
    uart_send(c);
    #endif
    #ifdef REQ_3_4
    _putc(c);
    #endif
    return;
}

int readline(char *buf, int maxlen) {
    int num = 0;
    while (num < maxlen - 1) {
        char c = uart_recv();
        uart_send(c);

        if (c == '\n' || c == '\0' || c == '\r') {
            if (c == '\r') uart_send('\n');
            break;
        }
        buf[num] = c;
        num++;
    }
    buf[num] = '\0';
    return num;
}

void uart_send_int(int number) {
    if (number == 0) {
        uart_send_string("0");
        return;
    }
    int i = 0, j; 
    char str[100];
    while (number) { 
        str[i++] = (number % 10) + '0'; 
        number = number / 10; 
    } 
    reserve(str, i);
    str[i] = '\0';
    uart_send_string(str);
    return ; 
}

void uart_send_hex(unsigned long number) {
    char buffer[11];
    int i;
    buffer[0] = '0';
    buffer[1] = 'x'; 
    buffer[2] = (char)((number >> 28) & 0xF);
    buffer[3] = (char)((number >> 24) & 0xF);
    buffer[4] = (char)((number >> 20) & 0xF);
    buffer[5] = (char)((number >> 16) & 0xF);
    buffer[6] = (char)((number >> 12) & 0xF);
    buffer[7] = (char)((number >> 8) & 0xF);
    buffer[8] = (char)((number >> 4) & 0xF);
    buffer[9] = (char)(number & 0xF);
    for (i = 2 ; i < 10 ; i++) {
        if (buffer[i] < 10) {
            buffer[i] += 48;
        }
        else {
            buffer[i] += 55;
        }
    }
    buffer[10] = '\0';
    uart_send_string(buffer);
    return;
}