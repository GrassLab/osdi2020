#ifndef TIMER
#define TIMER

extern void core_timer_enable ();
extern void core_timer_handler ();
void local_timer_handler ();
void local_timer_init ();
#endif /* ifndef TIMER */
