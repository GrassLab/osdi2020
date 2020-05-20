#include "user_lib.h"

void test_command1() { // test fork functionality
  int cnt = 0;
  if (user_fork() == 0) {
    user_fork();
    user_fork();
    while (cnt < 10) {
      // address should be the same across tasks, but the cnt should be
      // increased indepndently
      user_println("task id: %d, sp: 0x%x cnt: %d", get_taskid(), &cnt, cnt++);
      user_delay(10000000);
    }
    user_exit(); // all childs exit
  }
}


void test_command2() { // test page fault
  if (user_fork() == 0) {
    int *a = 0x0; // a non-mapped address.
    *a = 100;     // trigger simple page fault, child will died here
    user_exit();
  }
}


void test_command3() { // test page reclaim.
  user_println("====== test command 3 ======");
  user_println("Remaining page frames : %d",
               remain_page_num()); // get number of remaining page frames from
                                   // kernel by system call.
}

#define SWITCH_CONTINUE(buf, str, func)                                        \
  {                                                                            \
    if (strcmp(str, buf) == 0) {                                               \
      func();                                                                  \
      continue;                                                                \
    }                                                                          \
  }

int getcmd(char *buf, int nbuf);

void help() {
  user_write("Shell Usage:\r\n"
             "  help       Display this information.\r\n"
             "  hello      Display \"Hello World!\".\r\n"
             "  mem        Display memory status.\r\n"
             "  exit       call system exit.\r\n"
             "  reboot     Reboot.\r\n");
}

void hello() { user_write("Hello World!\r\n"); }

void exit() { user_exit(); }

void show_get_taskid() {
  int pid = get_taskid();
  user_println("task id: %d", pid);
}

void show_remain_page_num() {
  int rmp = remain_page_num();
  user_println("remain page number: %d", rmp);
}

void shell() {
  char buf[1024] = {0,};

  while (1) {
    if (getcmd(buf, sizeof(buf)) == -1)
      continue;
    SWITCH_CONTINUE(buf, "help", help);
    SWITCH_CONTINUE(buf, "hello", hello);
    SWITCH_CONTINUE(buf, "mem", user_mem_status);
    SWITCH_CONTINUE(buf, "taskid", show_get_taskid);
    SWITCH_CONTINUE(buf, "rmp", show_remain_page_num);
    SWITCH_CONTINUE(buf, "exit", user_exit);
    /* SWITCH_CONTINUE(buf, "reboot",    reset); */

    user_println("[ERR] command `%s' not found\r\n", buf);
  }

  return;
}

int getcmd(char *buf, int nbuf) {
  user_write("# ");

  memset(buf, 0, nbuf);
  char *p_buf = buf;

  /* read from uart to buf until newline */
  char c;
  while ((c = user_read()) != '\r') {
    if (c == 127 || c == 8) { /* backspace or delete */
      /* display */
      if (p_buf != buf) {
        /* display */
        user_write("\b \b");
        /* modified the buffer */
        *buf-- = 0;
      }
    } else {
      char b[2] = {c, 0};
      user_write(b);
      *buf++ = c;
    }
  }
  user_write("\r\n");
  *buf = 0;

  return p_buf[0] == 0 ? -1 : 0;
}

int main() {

  user_println("~~~~~~~~~~ test command 1 ~~~~~~~~~~");
  test_command1();

  user_delay(500000000);
  user_println("~~~~~~~~~~ test command 2 ~~~~~~~~~~");
  test_command2();

  user_delay(500000000);
  user_println("~~~~~~~~~~ test command 3 ~~~~~~~~~~");
  test_command3();

  if (user_fork() == 0) {
    shell();
  }

  return 0;
}
