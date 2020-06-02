
#ifndef __INFO_H__
#define __INFO_H__

int get_board_revision(unsigned int *mbox);
int get_arm_memaddr(unsigned int *mbox);
int get_vc_memaddr(unsigned int *mbox);
void get_current_el();

#endif
