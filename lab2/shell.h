#ifndef SHELL
#define SHELL

#define CMD_SIZE 0x20

void shell_interactive ();
double get_time ();
void reset (int tick);
void cancel_reset ();
void hardware ();
void picture ();
void loadimg (unsigned long address);
void ftoa (double val, char *buf);

#endif /* ifndef SHELL */
