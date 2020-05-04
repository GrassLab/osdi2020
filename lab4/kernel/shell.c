#include "miniuart.h"
#include "libc.h"
#include "mbox.h"
#include "timer.h"
#include "framebuffer.h"
#include "bh.h"
#include "sys.h"
#include "sched.h"
#include "signal.h"



int getcmd(char *buf, int nbuf);

#define SWITCH_CONTINUE(buf, str, func)                                        \
  {                                                                            \
    if (sstrcmp(str, buf) == 0) {                                              \
      func();                                                                  \
      continue;                                                                \
    }                                                                          \
  }

/* === The functionality of the shell === */
/* help: display the help message */
void help() {
  uart_puts("Shell Usage:\r\n"
            "  help       Display this information.\r\n"
            "  hello      Display \"Hello World!\".\r\n"
            "  timestamp  Display current timestamp.\r\n"
            "  exec       Show the exec for example.\r\n"
            "  exit       call system exit.\r\n"
            "  reboot     Reboot.\r\n"
            );
}

/* hello: display hello world */
void hello() { uart_puts("Hello World!\r\n"); }

/* reboot: reset the power of the board */
void reset() {
#define MMIO_BASE 0x3F000000
#define PM_RSTC ((volatile unsigned int *)(MMIO_BASE + 0x0010001c))
#define PM_RSTS ((volatile unsigned int *)(MMIO_BASE + 0x00100020))
#define PM_WDOG ((volatile unsigned int *)(MMIO_BASE + 0x00100024))
#define PM_WDOG_MAGIC 0x5a000000
#define PM_RSTC_FULLRST 0x00000020
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

/* getel: get the current exception level */
void get_current_el() {
  int el;
  asm volatile("mrs %0, CurrentEL\n"
               "lsr %0, %0, #2" : "=r"(el));
  uart_println("Current exception level: %d", el);
}

/* exc: issue the svc #1 */
void svc1() { asm volatile("svc #1"); }

/* irq: local/core timer interrupt */
void timer_interrupt() {
  local_timer_init();
  core_timer_enable();
}

void core_timer_interrupt() {
  core_timer_enable();
}

void local_timer_interrupt() {
  local_timer_init();
}


void bottom_half_mode() {
  bh_mod_mask = 1;
  core_timer_enable_w4sec();
}

/* get the information of the cpu */
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
  mbox[2] = MBOX_TAG_GETVCMEM;
  mbox[3] = 8;                 /* buffer size */
  mbox[4] = 0;
  /* 5-6 is reserve for output buffer */
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

void get_arm_memory() {
  /* buffer size in bytes (length of message) */
  mbox[0] = 8 * sizeof(unsigned int);

  /* Request message */
  mbox[1] = MBOX_REQUEST;

  /* ====== /Tags begin ====== */
  mbox[2] = MBOX_TAG_GETARMMEM;
  mbox[3] = 8;                 /* buffer size */
  mbox[4] = 0;
  /* 5-6 is reserve for output buffer */
  mbox[5] = 0; /* base address */
  mbox[6] = 0; /* size in bytes */
  /* the tag last for notify the mailbox */
  mbox[7] = MBOX_TAG_LAST;
  /* ====== Tags end/ ======== */

  /* send the message to the GPU and receive answer */
  if (mbox_call(MBOX_CH_PROP)) {
    uart_println("ARM base addr: 0x%x size 0x%x", mbox[5], mbox[6]);
  } else {
    uart_println("Unable to qery ARM memory!");
  }
}

int program1_pid = -1;

void program1() {
  while (1) {
    call_sys_write("program1 type: \"kill\" to end this program.\r\n");
    delay(10000000);
  }
  call_sys_exit();
}

void exec_example() {
  int pid = call_sys_fork();
  if (pid == 0) {
    call_sys_exec(program1);
  } else {
    program1_pid = pid;
  }
}

void signal_example() {
  if (program1_pid > 0) {
    send_signal(program1_pid, SIGKILL);
  }
}


void shell() {
  get_board_revision();
  get_arm_memory();
  get_vc_memory();

  static char buf[1024];

  while (1) {
    if (getcmd(buf, sizeof(buf)) == -1)
      continue;
    SWITCH_CONTINUE(buf, "help",      help);
    SWITCH_CONTINUE(buf, "hello",     hello);
    SWITCH_CONTINUE(buf, "timestamp", call_sys_timestamp);
    SWITCH_CONTINUE(buf, "show",      lfb_showpicture);
    SWITCH_CONTINUE(buf, "exec",      exec_example);
    SWITCH_CONTINUE(buf, "kill",      signal_example);
    SWITCH_CONTINUE(buf, "exit",      call_sys_exit);
    SWITCH_CONTINUE(buf, "reboot",    reset);

    uart_println("[ERR] command `%s` not found", buf);
  }

  call_sys_exit();
}

int getcmd(char *buf, int nbuf) {
  uart_puts("# ");

  memset(buf, 0, nbuf);
  char *p_buf = buf;

  /* read from uart to buf until newline */
  char c;
  while ((c = call_sys_read()) != '\r') {
    if (c == 127 || c == 8) { /* backspace or delete */
      /* display */
      if (p_buf != buf) {
        /* display */
        uart_puts("\b \b");
        /* modified the buffer */
        *buf-- = 0;
      }
    } else {
      uart_send(c);
      *buf++ = c;
    }
  }
  uart_puts("\r\n");
  *buf = 0;

  return p_buf[0] == 0 ? -1 : 0;
}
