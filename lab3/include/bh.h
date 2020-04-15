#pragma once

int bh_mod_mask;
char bh_mask[32];
char bh_active[32];

typedef void (*bh_handler)();

bh_handler bh_base[32];

void bottom_half_enable();
bh_handler extract_bh_handler();
void push_bh_handle(bh_handler job, int priority);
