#ifndef __MINIUART_H__
#define __MINIUART_H__

void miniuart_init(void);
char miniuart_getc(int echo);
char miniuart_putc(const char c);
void miniuart_puts(const char * string);
void miniuart_gets(char * string, char delimiter, unsigned length);

#endif

