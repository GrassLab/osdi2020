#ifndef SHELL
#define SHELL

#define CMD_SIZE 0x20

void shell_interactive ();
void print_time ();
void reset (int tick);
void cancel_reset ();
void hardware ();
void picture ();

#endif /* ifndef SHELL */
