extern void core_timer_enable ();
extern void core_timer_handler ();
void local_timer_handler ();
void local_timer_init ();
#define CORE0_TIMER_IRQ_CTRL (unsigned int *)0x40000040
#define LOCAL_TIMER_CONTROL_REG (unsigned int *)0x40000034
#define LOCAL_TIMER_IRQ_CLR (unsigned int *)0x40000038
