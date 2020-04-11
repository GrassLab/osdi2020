#ifndef EXCEPTION_H_
#define EXCEPTION_H_

extern unsigned vector_table[];

void exception_init(void);
void curr_el_spx_sync_handler(void);

#endif // EXCEPTION_H_
