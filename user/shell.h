#ifndef USER_SHELL_H_
#define USER_SHELL_H_

#define MAX_CMD_LEN 128

struct command {
  const char *name;
  const char *help;
  void(*handler)(void);
};

void help(void);
void test_command1(void);
void test_command3(void);

#endif // USER_SHELL_H_
