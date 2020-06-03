#include "typedef.h"

extern uint64_t arm_memory_start, arm_memory_end;
extern uint64_t vc_memory_start, vc_memory_end;

int mbox_call(unsigned int* mbox, unsigned char channel);
void mbox_board_revision();
void mbox_vc_memory();
void mbox_arm_memory();