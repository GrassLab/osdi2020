#ifndef STACKGUARD
#define STACKGUARD

void *__stack_chk_guard;
void __stack_chk_fail (void);

#endif /* ifndef STACKGUARD */
