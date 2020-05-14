#pragma once

extern int bh_mod_mask;

typedef void (*bh_handler)();


void bottom_half_enable();
bh_handler extract_bh_handler();
void push_bh_handle(bh_handler job, int priority);
