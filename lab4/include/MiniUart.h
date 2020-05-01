#ifndef __MINI_UART_H
#define __MINI_UART_H

void initUART(void);
void sendUART(char c);
char recvUART(void);
void sendStringUART(const char *str);
void sendHexUART(unsigned int hex);
void delay(unsigned int clock);

// ---- For User Mode --------------------
char readUART(void);
void writeUART(char c);
void writeStringUART(const char *str);
void writeHexUART(unsigned int hex);

#endif
