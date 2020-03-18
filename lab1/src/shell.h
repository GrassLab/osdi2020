#ifndef SHELL
#define SHELL

#define PM_PASSWORD 0x5a000000
#define PM_RSTC (volatile unsigned int*)0x3F10001c
#define PM_WDOG (volatile unsigned int*)0x3F100024

void welcome_msg();

void run(char *command);

void help();

void hello();

void timestamp();

void reboot(int tick);

#endif
