#include "framebuffer.h"
#include "libc.h"
#include "loader.h"
#include "mbox.h"
#include "shell.h"
#include "uart.h"
#include "unittest.h"

#define MMIO_BASE 0x3F000000
#define PM_RSTC ((volatile unsigned int *)(MMIO_BASE + 0x0010001c))
#define PM_RSTS ((volatile unsigned int *)(MMIO_BASE + 0x00100020))
#define PM_WDOG ((volatile unsigned int *)(MMIO_BASE + 0x00100024))
#define PM_WDOG_MAGIC 0x5a000000
#define PM_RSTC_FULLRST 0x00000020

extern uint8_t __start;
extern uint8_t __end;

void hello() { uart_puts("Hello World!\r\n"); }

void help() {
  uart_puts("Shell Usage:\r\n"
            "  help       Display this information.\r\n"
            "  hello      Display \"Hello World!\"\r\n"
            "  timestamp  Display current timestamp.\r\n"
            "  reboot     Reboot.\r\n");
}

void wait_msec(unsigned int n) {
  register unsigned long f, t, r;
  // get the current counter frequency
  asm volatile("mrs %0, cntfrq_el0" : "=r"(f));
  // read the current counter
  asm volatile("mrs %0, cntpct_el0" : "=r"(t));
  // calculate expire value for counter
  t += ((f / 1000) * n) / 1000;
  do {
    asm volatile("mrs %0, cntpct_el0" : "=r"(r));
  } while (r < t);
}

float get_system_timer() {
  register unsigned long long f, t;

  /* get the current counter frequency */
  asm volatile("mrs %0, cntfrq_el0" : "=r"(f));
  /* read the current counter */
  asm volatile("mrs %0, cntpct_el0" : "=r"(t));

  /* time = timer counter / timer frequency */
  unsigned long long i_part = t / f;
  unsigned long long f_part = t * 100000000 / f % 100000000;

  uart_println("[%d.%d]", i_part, f_part);
  return t / f;
}

void reset() {
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

#define SWITCH_CONTINUE(buf, str, func)                                        \
  {                                                                            \
    if (sstrcmp(str, buf) == 0) {                                              \
      func();                                                                  \
      continue;                                                                \
    }                                                                          \
  }

int bss_test() {
  register unsigned long *beg, *end;
  asm volatile("ldr %0, =__bss_start" : "=r"(beg));
  asm volatile("ldr %0, =__bss_end" : "=r"(end));

  for (unsigned long *p = beg; p != end; ++p) {
    if (*p != 0) {
      return -1;
    }
  }
  return 0;
}

int stack_test() {
  extern char __stack_top;
  unsigned long *stack_top = (unsigned long *)&__stack_top;

  unsigned long sp;
  asm volatile("mov %0, sp" : "=r"(sp));
  return (unsigned long *)sp < stack_top ? 0 : -1;
}

int timestamp_test() {
  float a = get_system_timer();
  wait_msec(1000000);
  float b = get_system_timer();
  return (b > a && b - a < 2) ? 0 : -1;
}

/* check the copied kernel is correct */
int copy_test() {
  size_t kernel_size = (&__end - &__start);
  uint8_t *original_addr = &__start;
  uint8_t *new_addr = (uint8_t *)(BOOT_ADDR);
  int flag = 0;
  while (kernel_size-- != 0) {
    if (*(uint8_t *)new_addr++ != *(uint8_t *)original_addr++) {
      /* uart_println("[copy test] differ @ %x", original_addr-1); */
      flag = -1;
    }
  }
  return flag;
}

void get_board_revision() {
  /* buffer size in bytes (length of message) */
  mbox[0] = 7 * sizeof(unsigned int);

  mbox[1] = MBOX_REQUEST; /* Request message */

  /* ====== /Tags begin ====== */
  mbox[2] = MBOX_TAG_GETREV; /* get serial number command */
  mbox[3] = 4;               /* buffer size */
  mbox[4] = 0;
  mbox[5] = 0;
  /* the tag last for notify the mailbox */
  mbox[6] = MBOX_TAG_LAST;
  /* ====== Tags end/ ======== */

  /* send the message to the GPU and receive answer */
  if (mbox_call(MBOX_CH_PROP)) {
    uart_println("Revision number is: 0x%x", mbox[5]);
  } else {
    uart_println("Unable to qery revision!");
  }
}

void get_vc_memory() {
  /* buffer size in bytes (length of message) */
  mbox[0] = 8 * sizeof(unsigned int);

  /* Request message */
  mbox[1] = MBOX_REQUEST;

  /* ====== /Tags begin ====== */
  mbox[2] = MBOX_TAG_GETVCMEM; /* get serial number command */
  mbox[3] = 8;                 /* buffer size */
  mbox[4] = 0;
  /* 5-6 is reserve for output buffer (because the serial number 8 bytes is
   * required) */
  mbox[5] = 0; /* base address */
  mbox[6] = 0; /* size in bytes */
  /* the tag last for notify the mailbox */
  mbox[7] = MBOX_TAG_LAST;
  /* ====== Tags end/ ======== */

  /* send the message to the GPU and receive answer */
  if (mbox_call(MBOX_CH_PROP)) {
    uart_println("VC core base addr: 0x%x size 0x%x", mbox[5], mbox[6]);
  } else {
    uart_println("Unable to qery vc memory!");
  }
}

void loadkernel() {
  while (1) {
    loadimg();
  RESEND:
    uart_println("[ERR] load image failed, please type 'r' to resend kernel"
                 " or type ctrl+c to cancel");
    char c = uart_getc();
    if (c == 0x3) { /* ctrl+c */
      break;
    } else if (c == 'r') {
      continue;
    } else {
      goto RESEND;
    }
  }
}

const unsigned int clock_rate = 4000000;
const unsigned int baud_rate  = 115200;

int main() {
  size_t kernel_size = (&__end - &__start);

  // just ensure that the program running at 0x200000
  // is not using the memory in original one
  memset(&__start, 0, kernel_size);

  // set up serial console
  uart_init(clock_rate, baud_rate);

  // set up framebuffer
  lfb_init();

  get_board_revision();
  get_vc_memory();

  // display a pixmap
  /* lfb_showpicture(); */

  /* shell */
  static char buf[1024];
  while (1) {
    if (getcmd(buf, sizeof(buf)) == -1)
      continue;
    SWITCH_CONTINUE(buf, "help", help);
    SWITCH_CONTINUE(buf, "hello", hello);
    SWITCH_CONTINUE(buf, "timestamp", get_system_timer);
    SWITCH_CONTINUE(buf, "show", lfb_showpicture);
    SWITCH_CONTINUE(buf, "reboot", reset);
    SWITCH_CONTINUE(buf, "loadimg", loadkernel);

    uart_println("[ERR] command `%s` not found", buf);
  }
}

__attribute__((section(".text.relocate"))) void relocate() {
  uart_init(clock_rate, baud_rate);



  uart_println("-----------------------------------\r\n"
               "                                   \r\n"
               "     OSDI2020 UART Bootloader      \r\n"
               "                                   \r\n"
               "-----------------------------------",
               BOOT_ADDR);

  uart_println("Init PL011 done [clock rate: %dHz]", uart_getrate());

  uart_println("\033[0;32mcopying itself to 0x%x address \033[0m", BOOT_ADDR);

  size_t kernel_size = (&__end - &__start);
  uint8_t *new_addr = (uint8_t *)(BOOT_ADDR);

  uart_println("start copying from:              \r\n"
               "   __start: 0x%x to __end: 0x%x  \r\n"
               "   @ new_addr %x                 \r\n"
               "   w/t kernel_size 0x%x",
               &__start, &__end, new_addr, kernel_size);

  memcpy(new_addr, &__start, kernel_size);

  kentry_t kentry = (kentry_t)new_addr;

  unittest(copy_test, "copy_test", "Ensure the copied kernel is correct");

  uart_println("executing the kernel @ %x", new_addr);
  uart_println("-----------------------------------");

  kentry();
}
