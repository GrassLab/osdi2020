#ifndef __MINIUART_H__
#define __MINIUART_H__

void miniuart_init(void);
char miniuart_getc(int echo);
char miniuart_putc(char c);
void miniuart_puts(char * string);
void miniuart_gets(char * string, char delimiter, unsigned length);

#endif

