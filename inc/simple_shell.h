#ifndef	_SIMPLE_SHELL_H
#define	_SIMPLE_SHELL_H

#define NUM_COMMAND 5
#define MAX_COMMAND_LENGTH 128

void run_shell(void);
int read_command(char*, unsigned int);
int parse_command(char*);

void cmd_help();
void cmd_hello();
void cmd_timestamp();
void cmd_reboot();
void cmd_loadimg();

#endif//_SIMPLE_SHELL_H