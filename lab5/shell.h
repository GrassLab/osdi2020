#ifndef __SHELL_H__
#define __SHELL_H__

void shell(void);
void _shell_parser();
int shell_hello(char * string_buffer);
int shell_help(char * string_buffer);
int shell_exc(char * string_buffer);
int shell_irq(char * string_buffer);

extern char __executable_start;
extern char __etext;

#endif

