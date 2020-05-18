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
    char buf[MAX_CMD_LEN];
    uart_read(buf, MAX_CMD_LEN);

    for (size_t i = 0; cmds[i].name != NULL; ++i) {
      if (!strcmp(buf, cmds[i].name)) {
        cmds[i].handler();
        break;
      }
    }
  }
}
