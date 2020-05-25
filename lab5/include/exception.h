#ifndef _EXCEPTION_H
#define _EXCEPTION_H

void el2_current_sp2_sync_message(unsigned long elr_el2, unsigned long esr_el2);
void el2_print();
void
_el2_exception_handler(unsigned long type, unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far);
void
_context_switch_msg();

#endif