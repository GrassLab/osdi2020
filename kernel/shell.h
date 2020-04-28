#ifndef SHELL
#define SHELL

#define CMD_SIZE 0x20
#include <stddef.h>

void shell_interactive ();
double get_time ();
void reset (int tick);
void cancel_reset ();
void hardware ();
void picture (char buf[3]);
void loadimg (unsigned long address)
  __attribute__ ((section (".bootloader")));
void loadimg_jmp (void *address, unsigned long img_size)
  __attribute__ ((section (".bootloader")));
void sys_get_time (size_t *cnt, size_t *freq);
#endif /* ifndef SHELL */
