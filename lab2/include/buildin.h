#ifndef _BUILDIN_INCLUDE_H_
#define _BUILDIN_INCLUDE_H_

#define NULL (void *)0
#include "stdint.h"
void hello();
void reset();
void cancel_reset();
void getTimestamp();
void print_info();
void get_board_revision();
void show_splash();
void load_kernel();
#endif