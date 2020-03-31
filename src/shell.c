#include "shell.h"

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

void put_shell() {
  // shell command
  char *help = "help";
  char *hello = "hello";
  char *time = "timestamp";
  char *reboot = "reboot";
  char *board = "board revision";
  char *vc= "VC base";
  char *loadimage = "loadimg";

  char str[100];
  int p = 0;
  char chr;

  uart_puts("# ");
  while((chr = uart_getc()) != '\n') {
    if (chr == 127) {
      uart_puts("\b \b");
      p--;
    } else {
      str[p] = chr;
      uart_send(chr);
      p++;
    }
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
  } else if (strcmp(str, board) == 0) {
  // get the board's unique serial number with a mailbox call
    mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message

    mbox[2] = MBOX_TAG_GETBOARD;   // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    // send the message to the GPU and receive answer
    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My board revision is: ");
        //uart_hex(mbox[6]);
        uart_hex(mbox[5]);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query board revision!\n");
    }
  } else if (strcmp(str, vc) == 0) {
  // get the board's unique serial number with a mailbox call
    mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message

    mbox[2] = MBOX_TAG_GETVCBASE;   // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    // send the message to the GPU and receive answer
    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My VC base is: ");
        //uart_hex(mbox[6]);
        uart_hex(mbox[5]);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query VC base!\n");
    }
  } else if (strcmp(str, reboot) == 0)
    reset(0);
  else if (strcmp(str, loadimage) == 0)
    loadimg();
  else {
    uart_puts("command not found: ");
    uart_puts(str);
    uart_puts("\n");
  }
}
