#include "user_sys.h"
#include "user_lib.h"
#include "user.h"
#include "printf.h"

void loop(char* str)
{
    char buf[2] = {""};
    while (1){
        for (int i = 0; i < 5; i++){
            buf[0] = str[i];
            call_sys_write(buf);
            user_delay(1000000);
        }
    }
}

#define SWITCH_CONTINUE(buf, str, func)                                        \
  {                                                                            \
    if (sstrcmp(str, buf) == 0) {                                              \
      func();                                                                  \
      continue;                                                                \
    }                                                                          \
  }

int getcmd(char *buf, int nbuf);

void help() {
 call_sys_write("Shell Usage:\r\n"
            "  help       Display this information.\r\n"
            "  hello      Display \"Hello World!\".\r\n"
            "  exit       call system exit.\r\n"
            "  reboot     Reboot.\r\n"
            );
}

void hello() { call_sys_write("Hello World!\r\n"); }

void exit() { call_sys_exit(); }

void shell() {
  char buf[1024];

  while (1) {
    if (getcmd(buf, sizeof(buf)) == -1)
      continue;
    SWITCH_CONTINUE(buf, "help",      help);
    SWITCH_CONTINUE(buf, "hello",     hello);
    SWITCH_CONTINUE(buf, "exit",      call_sys_exit);
    /* SWITCH_CONTINUE(buf, "reboot",    reset); */

    call_sys_write("[ERR] command not found\r\n");
  }

  call_sys_exit();
}

void test_command1() { // test fork functionality
  int cnt = 0;
  if (call_sys_fork() == 0) {
    call_sys_fork();
    call_sys_fork();
    while (cnt < 10) {
      // address should be the same across tasks, but the cnt should be
      // increased indepndently
      cnt++;
    }
    call_sys_exit(); // all childs exit
  }
}


void user_process() {
  call_sys_write("====== test_command 1 =====");
  call_sys_write("====== test_command 2 =====");
  call_sys_write("====== test_command 3 =====");
  if (call_sys_fork() == 0) {
    test_command1();
    call_sys_exit();
  }

  call_sys_write("====== test_command 3 =====");

  call_sys_exit();
  /* call_sys_write("User process\n\r"); */
  /* int pid = call_sys_fork(); */
  /* if (pid < 0) { */

  /*   call_sys_write("Error during fork\n\r"); */
  /*   call_sys_exit(); */
  /*   return; */
  /* } */
  /* if (pid == 0) { */
  /*   shell(); */
  /* } else { */
  /*   call_sys_exit(); */
  /* } */
}



int getcmd(char *buf, int nbuf) {
  call_sys_write("# ");

  mmemset(buf, 0, nbuf);
  char *p_buf = buf;

  /* read from uart to buf until newline */
  char c;
  while ((c = call_sys_read()) != '\r') {
    if (c == 127 || c == 8) { /* backspace or delete */
      /* display */
      if (p_buf != buf) {
        /* display */
        call_sys_write("\b \b");
        /* modified the buffer */
        *buf-- = 0;
      }
    } else {
      char b[2] = {c, 0};
      call_sys_write(b);
      *buf++ = c;
    }
  }
  call_sys_write("\r\n");
  *buf = 0;

  return p_buf[0] == 0 ? -1 : 0;
}
