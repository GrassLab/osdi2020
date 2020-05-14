#pragma once

typedef void (*bh_handler)();

extern int bh_mod_mask;

void bottom_half_enable();
bh_handler extract_bh_handler();
void push_bh_handle(bh_handler job, int priority);
