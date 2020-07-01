#pragma once

typedef void (*bh_handler)();


extern int bh_mod_mask;
extern char bh_mask[32];
extern char bh_active[32];
extern bh_handler bh_base[32];





void bottom_half_enable();
bh_handler extract_bh_handler();
void push_bh_handle(bh_handler job, int priority);
