#include "uart.h"
#include "ftoa.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)0x3F10001c)
#define PM_WDOG ((volatile unsigned int*)0x3F100024)

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
  *PM_WDOG = PM_PASSWORD | 10; // number of watchdog tick
}

void cancel_reset() {
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}

int strcmp (const char *p1, const char *p2) {
  char *s1 = (char *) p1;
  char *s2 = (char *) p2;
  char c1, c2;

  do {
      c1 = (char) *s1++;
      c2 = (char) *s2++;
      if (c1 == '\0')
	return c1 - c2;
  } while (c1 == c2);

  return c1 - c2;
}

void main() {
  // set up serial console
  uart_init();
  uart_flush();

  char *osdi = "OSDI Welcome\n";
  uart_puts(osdi);

  // echo everything back
  char *help = "help";
  char *hello = "hello";
  char *time = "timestamp";
  char *reboot = "reboot";
  char str[100];
  for(;;) {
    int p = 0;
    char chr;
    uart_puts("# ");
    while((chr = uart_getc()) != '\n') {
      str[p] = chr;
      uart_send(str[p]);
      p++;
    }
    str[p] = '\0';
    uart_puts("\n");
    if (strcmp(str, help) == 0)
      uart_puts("help : print all available commands\nhello : print Hello World!\nreboot : reboot rpi3\ntimestamp : print current timestamp\n");
    else if (strcmp(str, hello) == 0)
      uart_puts("Hello World!\n");
    else if (strcmp(str, time) == 0) {
      long f, c;
      double t;
      char buf[30];
      // get the current counter frequency
      asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
      // read the current counter
      asm volatile ("mrs %0, cntpct_el0" : "=r"(c));
      // calculate expire value for counter
      t = c / (double)f;
      uart_puts("[");
      uart_puts(ftoa(t, buf, 5));
      uart_puts("]\n");
    }
    else if (strcmp(str, reboot) == 0)
      reset(0);
    else {
      uart_puts("command not found: ");
      uart_puts(str);
      uart_puts("\n");
    }
  }
}
