#include "mbox.h"
#include "uart.h"

#define MMIO_BASE 0x3F000000
#define PM_RSTC ((volatile unsigned int *)(MMIO_BASE + 0x0010001c))
#define PM_RSTS ((volatile unsigned int *)(MMIO_BASE + 0x00100020))
#define PM_WDOG ((volatile unsigned int *)(MMIO_BASE + 0x00100024))
#define PM_WDOG_MAGIC 0x5a000000
#define PM_RSTC_FULLRST 0x00000020

char *welcome = " \
░░░░░░░░░░░░░░░░▄░█▄░█▄▄▄░░\n \
█▀▀█░░█▀▀█░█▀▀█▄█▄█▄░█▄▄▄█░\n \
█▀▀█░░█▀▀█░█░░█░░▄█▄▄▄░░░░░\n \
█▀▀▀░░▀▀▀█░█░░█░▀▄▄▄█▄▄▄░░░\n \
█░▄▄▄▄█▄░█░▀▀▀▀░░█▄▄█▄▄█░░░\n \
█░░░▄▀█░░█░░░░░░░█▄▄█▄▄█░░░\n \
█░▄▀░▄█░▄█░░░░░░▄▀░░░░▄█░░░\n \
";
void reboot() {
  unsigned int r;
  // trigger a restart by instructing the GPU to boot from partition 0
  r = *PM_RSTS;
  r &= ~0xfffffaaa;
  *PM_RSTS = PM_WDOG_MAGIC | r; // boot from partition 0
  *PM_WDOG = PM_WDOG_MAGIC | 10;
  *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;

  while (1)
    ;
}

int my_strcpy(char *buf1, char *buf2) {
  int i;
  for (i = 0; *(buf1 + i) != '\x00'; i++) {
    if (*(buf1 + i) != *(buf2 + i)) {
      return 0;
    }
  }
  if (*(buf2 + i) != '\x00') {
    return 0;
  }
  return 1;
}

void my_itoa(long result) {
  long m = result;
  char ms[20] = {0};
  int count;
  for (count = 0; m; count++) {
    ms[count] = (m % 10) + '0';
    m = m / 10;
  }
  int i;
  char rms[20] = {0};
  for (i = count; i > 0; i--) {
    rms[count - i] = ms[i - 1];
  }
  uart_puts(rms);
  uart_puts(" ms\n");
}

void clearbuf(char *buf) {
  for (int i = 0; i < 100; i++) {
    *(buf + i) = 0;
  }
}

unsigned int get_system_frequency() {
  unsigned int res = 0;
  asm volatile("MRS %[result], CNTFRQ_EL0" : [result] "=r"(res));
  return res;
}

unsigned int get_system_count() {
  unsigned int res = 0;
  asm volatile("MRS %[result],  CNTPCT_EL0" : [result] "=r"(res));
  return res;
}

void compair(char *buf) {
  if (my_strcpy(buf, "hello")) {
    uart_puts("Hello World!\n");
  } else if (my_strcpy(buf, "help")) {
    uart_puts("help : print all  command\n");
    uart_puts("hello : say hello\n");
    uart_puts("timestamp : get time\n");
    uart_puts("reboot : reboot rpi3 \n");
  } else if (my_strcpy(buf, "timestamp")) {
    long countertimer = get_system_count();
    long frequency = get_system_frequency();
    countertimer = countertimer * 1000;
    long result = countertimer / frequency;
    my_itoa(result);
  } else if (my_strcpy(buf, "reboot")) {
    reboot();
  } else if (my_strcpy(buf, "version")) {
    board_revision();
  } else if (my_strcpy(buf, "vc_addr")) {
    vc_base_address();
  } else {
    uart_puts("unknow command ");
    uart_puts(buf);
    uart_puts(". \n");
  }
}

void main() {
  uart_init();
  uart_puts(welcome);
  uart_puts("# ");

  char commandbuf[100];
  clearbuf(commandbuf);
  int count = 0;
  while (1) {
    char get = uart_getc();
    uart_send(get);
    if (get != '\n') {
      commandbuf[count] = get;
      count++;
    } else {
      commandbuf[count] = '\x00';
      compair(commandbuf);
      uart_puts("# ");
      count = 0;
      clearbuf(commandbuf);
    }
  }
}
