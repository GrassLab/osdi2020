#ifndef	_SHELL_H
#define	_SHELL_H

#define NUM_COMMAND 7
#define MAX_COMMAND_LENGTH 128

void run_shell(void);
int read_command(char*, unsigned int);
int parse_command(char*);

void cmd_help();
void cmd_hello();
void cmd_timestamp();
void cmd_reboot();
void cmd_loadimg();
void cmd_exc();
void cmd_irq();

#endif//_SHELL_H