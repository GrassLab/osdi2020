#ifndef __MINI_UART_H__
#define __MINI_UART_H__

#define BASE 0x3F000000

#define AUXENB          (BASE + 0x00215004)
#define AUX_MU_CNTL_REG (BASE + 0x00215060)
#define AUX_MU_IER_REG  (BASE + 0x00215044)
#define AUX_MU_LCR_REG  (BASE + 0x0021504C)
#define AUX_MU_MCR_REG  (BASE + 0x00215050)
#define AUX_MU_BAUD_REG (BASE + 0x00215068)
#define AUX_MU_IIR_REG  (BASE + 0x00215048)
#define AUX_MU_CNTL_REG (BASE + 0x00215060)

#define AUX_MU_IO_REG   (BASE + 0x00215040)
#define AUX_MU_LSR_REG  (BASE + 0x00215054)


#define GPFSEL1         (BASE + 0x00200004)
#define GPSET0          (BASE + 0x0020001C)
#define GPCLR0          (BASE + 0x00200028)
#define GPPUD           (BASE + 0x00200094)
#define GPPUDCLK0       (BASE + 0x00200098)

void uart_send(char c);
char uart_recv(void);
void uart_flush(void);
void uart_send_string(char*);
void uart_init(void);

#endif
