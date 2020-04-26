#ifndef __MINI_UART_H
#define __MINI_UART_H

void initUART(void);
void sendUART(char c);
char recvUART(void);
void sendStringUART(const char *str);
void sendHexUART(unsigned int hex);
void delay(unsigned int clock);

#endif
