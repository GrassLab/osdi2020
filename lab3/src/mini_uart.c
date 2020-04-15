#include "utils.h"
#include "peripherals/mini_uart.h"
#include "peripherals/gpio.h"
#define BUFFER_SIZE 100
#define CMD_BUFFER_SIZE 100

char uart_buffer[BUFFER_SIZE];
char cmd_buffer[CMD_BUFFER_SIZE];
unsigned int wr_buffer_index = 0;
unsigned int rd_buffer_index = 0;
unsigned int cmd_index = 0;
unsigned int cmd_flag  = 0;
int transmit_interrupt_open = 0;


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
    // put into buffer
	for (int i = 0; str[i] != '\0'; i ++) {
		uart_buffer[wr_buffer_index++] = str[i];
        if (wr_buffer_index == BUFFER_SIZE) {
            wr_buffer_index = 0;
        }
	}
    // open uart_transmit interrupt
    if (transmit_interrupt_open == 0){
        put32(AUX_MU_IER_REG, 3);
        transmit_interrupt_open = 1;
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

void handle_uart_irq()
{
    unsigned int id = get32(AUX_MU_IIR_REG);
    if((id & 0x06) == 0x04)
	{
        // open tansmit interrupt
        if (transmit_interrupt_open == 0){
            put32(AUX_MU_IER_REG, 3);
            transmit_interrupt_open = 1;
        }
        while(get32(AUX_MU_LSR_REG)&0x01) {
            char c;
            c = get32(AUX_MU_IO_REG)&0xFF;
            if (cmd_flag == 1) {
                cmd_flag = 0;
                cmd_index = 0;
            }
            if ( c == '\r') {
                cmd_buffer[cmd_index++] = '\0';
                cmd_index = 0;
                cmd_flag  = 1;
                uart_buffer[wr_buffer_index++] = '\r';
                if (wr_buffer_index == BUFFER_SIZE) {
                    uart_buffer[0] = '\n';
                    wr_buffer_index = 1;
                }
                else {
                    uart_buffer[wr_buffer_index++] = '\n';
                }
            }
            else{
                cmd_buffer[cmd_index++] = c;
                uart_buffer[wr_buffer_index++] = c;
            }
            if(wr_buffer_index == BUFFER_SIZE)
                wr_buffer_index = 0;    
        }
	}
    if((id & 0x06) == 0x02)
	{
        while(get32(AUX_MU_LSR_REG)&0x20) {
            if(rd_buffer_index == wr_buffer_index) {
                // close transmit interrupt
                put32(AUX_MU_IER_REG, 1); 
                transmit_interrupt_open = 0;
                return;
            }
            char c = uart_buffer[rd_buffer_index++];
            put32(AUX_MU_IO_REG,c);
            if(rd_buffer_index == BUFFER_SIZE)
                rd_buffer_index = 0;    
        }
	}

    return;
}


// This function is required by printf function
void putc ( void* p, char c)
{
	uart_send(c);
}

int readline(char *buf, int maxlen) {
    int num = 0;
    while (num < maxlen - 1) {
        char c = uart_recv();
        if (c == '\n' || c == '\0' || c == '\r') {
            break;
        }
        buf[num] = c;
        num++;
    }
    buf[num] = '\0';
    return num;
}

void uart_send_int(int number) {
    int i = 0, j; 
    char str[100];
    while (number) { 
        str[i++] = (number % 10) + '0'; 
        number = number / 10; 
    } 
    reserve(str, i);
    str[i] = '\r';
    str[i + 1] = '\n';
    str[i + 2] = '\0';
    uart_send_string(str);
    return ; 
}

void uart_send_hex(unsigned long number) {
    char buffer[12];
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
    buffer[10] = '\r';
    buffer[11] = '\n';
    buffer[12] = '\0';
    uart_send_string(buffer);
    return;
}