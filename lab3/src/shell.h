#ifndef SHELL
#define SHELL

#define PM_PASSWORD 0x5a000000
#define PM_RSTC (volatile unsigned int*)0x3F10001c
#define PM_WDOG (volatile unsigned int*)0x3F100024

#define CORE0_TIMER_IRQ_CTRL (volatile unsigned int*)0x40000040

// lab 1

void welcome_msg();

void run(char *command);

void help();

void hello();

void timestamp();

void reboot(int tick);

// lab 2

void get_board_revision();

void get_vc_base_address();

void loadimg();

void exc();

void irq();

#endif
