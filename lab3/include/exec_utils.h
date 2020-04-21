#include "config.h"

void reset(int tick);
void cancel_reset();
int recv_img_size();
void copy_self_kernel();
void copy_and_jump_to_kernel();

#define  TMP_KERNEL_ADDR  0x100000
extern char __bss_end[];
extern char start_begin[];
