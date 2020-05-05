#ifndef __SHELL_H__
#define __SHELL_H__

int busy_shell_loop(int el);
void irq_shell_loop(int el);
int shell_execute(char *, int el);
char *shell_stuff_line(char c, char **ptr, char *buffer);

#endif
