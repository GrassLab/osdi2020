#ifndef	_SIMPLE_SHELL_H
#define	_SIMPLE_SHELL_H

#define CommandNumber 4
#define MaxCommandLine 128

void run_shell(void);
int read_command(char*, unsigned int);
int parse_command(char*);
void print_prompt(void);

#endif /*_SIMPLE_SHELL_H*/