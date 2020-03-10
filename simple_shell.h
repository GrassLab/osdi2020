#ifndef	_SIMPLE_SHELL_H
#define	_SIMPLE_SHELL_H

#define NUM_COMMAND 4
#define MAX_COMMAND_LENGTH 128

void run_shell(void);
int read_command(char*, unsigned int);
int parse_command(char*);

void show_help();
void show_hello();
void show_timestamp();
void reboot_rpi3();

void print_prompt(void);


#endif /*_SIMPLE_SHELL_H*/