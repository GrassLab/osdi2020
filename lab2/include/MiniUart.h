#ifndef __MINI_UART_H
#define __MINI_UART_H

void initUART(void);
void sendUART(char c);
char recvUART(void);
void sendStringUART(const char *str);

#endif
