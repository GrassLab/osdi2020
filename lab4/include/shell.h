#ifndef __SHELL_H__
#define __SHELL_H__

int busy_shell_loop();
void irq_shell_loop();
int shell_execute(char *);
void shell_stuff_line(char);

#endif
