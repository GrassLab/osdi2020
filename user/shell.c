#include "user/lib/types.h"
#include "user/lib/ioutil.h"
#include "user/lib/string.h"
#include "user/lib/syscall.h"
#include "user/shell.h"

struct command cmds[] = {
  { "help", "Print the helping message\n", help },
  { "test1", "Test fork functionality\n", test_command1 },
  { "test3", "Test page reclaim\n", test_command3 },
  { NULL, NULL, NULL }
};

void delay(int t) {
  for (int i = 0; i < t; ++i) {}
}

void help(void) {
  for (size_t i = 0; cmds[i].name != NULL; ++i) {
    printf("%s: %s", cmds[i].name, cmds[i].help);
  }
}

void test_command1(void) {
  int cnt = 0;
  if(fork() == 0) {
    fork();
    fork();
    while(cnt < 10) {
      // address should be the same across tasks, but the cnt should be increased indepndently
      printf("task id: %u, sp: %#x cnt: %u\n", get_taskid(), &cnt, cnt++);
      delay(1000000);
    }
    // all childs exit
    exit(0);
  }
}

void test_command3(void) {
  // get number of remaining page frames from kernel by system call.
  printf("Remaining page frames : %u\n", get_remain_page_num());
}

int main(void) {
  while (true) {
    puts("> ");
    char buf[MAX_CMD_LEN];
    gets(buf);

    size_t i = 0;
    for (; cmds[i].name != NULL; ++i) {
      if (!strcmp(buf, cmds[i].name)) {
        cmds[i].handler();
        break;
      }
    }
    if (cmds[i].name == NULL) {
      printf("%s: command not found\n", buf);
    }
  }
}
