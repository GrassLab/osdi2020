#include "shell.h"
#include "uart.h"

/* itoa impl reference: */
/* https://stackoverflow.com/questions/3982320/convert-integer-to-string-without-access-to-libraries */
char* itoa(int val, int base){
  static char buf[32] = {0};
  int i = 30;
  for(; val && i ; --i, val /= base)
    buf[i] = "0123456789abcdef"[val % base];
  return &buf[i+1];
}

void hello() {
  uart_puts("Hello World!\r\n");
}

void help() {
  uart_puts("Shell Usage:\r\n");
  uart_puts("  help       Display this information.\r\n");
  uart_puts("  hello      Display \"Hello World!\"\r\n");
  uart_puts("  timestamp  Display current timestamp.\r\n");
  uart_puts("  reboot     Reboot.\r\n");
}

#define MMIO_BASE       0x3F000000
#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020


void get_system_timer() {
  register unsigned long long f, t;
  /* get the current counter frequency */
  asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
  /* read the current counter */
  asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
  /* time = timer counter / timer frequency */

  unsigned long long i_part = t / f;
  unsigned long long f_part = t * 100000000 / f % 100000000;

  uart_puts("[");
  char *i_part_str = itoa(i_part, 10);
  uart_puts(i_part_str);

  char *f_part_str = itoa(f_part, 10);
  uart_puts(".");
  uart_puts(f_part_str);
  uart_puts("]\r\n");
}


void reset()
{
  unsigned int r;
  // trigger a restart by instructing the GPU to boot from partition 0
  r = *PM_RSTS; r &= ~0xfffffaaa;
  *PM_RSTS = PM_WDOG_MAGIC | r;   // boot from partition 0
  *PM_WDOG = PM_WDOG_MAGIC | 10;
  *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;

  while (1)
    ;
}

#define SWITCH_CONTINUE(buf, str, func)         \
  {                                             \
    if (sstrcmp(str, buf) == 0) {               \
      func();                                   \
      continue;                                 \
    }                                           \
  }

int main() {
  // set up serial console
  uart_init();
  uart_flush();

  uart_puts("----------------\r\n"
            "    OSDI2020    \r\n"
            "----------------\r\n");

  /* shell */
  static char buf[100];
  while (1) {
    if (getcmd(buf, sizeof(buf)) == -1) {
      continue;
    }
    SWITCH_CONTINUE(buf, "hello", hello);
    SWITCH_CONTINUE(buf, "help", help);
    SWITCH_CONTINUE(buf, "reboot", reset);
    SWITCH_CONTINUE(buf, "timestamp", get_system_timer);

    uart_puts("[ERR] command `");
    uart_puts(buf);
    uart_puts("` not found\r\n");

  }
  return 0;
}
