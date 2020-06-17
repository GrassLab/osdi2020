#ifndef __MINI_UART_H
#define __MINI_UART_H

#include <stdint.h>

void initUART(void);
void sendUART(char c);
char recvUART(void);
void sendStringUART(const char *str);
void sendHexUART(uint64_t hex);
void delay(unsigned int clock);

// ---- For User Mode --------------------
char readUART(void);
void writeUART(char c);
void writeStringUART(const char *str);
void writeHexUART(uint64_t hex);

#endif
