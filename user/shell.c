#include "user/lib/types.h"
#include "user/lib/ioutil.h"
#include "user/lib/string.h"
#include "user/lib/syscall.h"
#include "user/shell.h"

struct command cmds[] = {
  { "help", "Print the helping message\n", help },
  { NULL, NULL, NULL }
};

void help(void) {
  for (size_t i = 0; cmds[i].name != NULL; ++i) {
    printf("%s: %s", cmds[i].name, cmds[i].help);
  }
}

int main(void) {
  while (true) {
    uart_write("> ", 2);
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
