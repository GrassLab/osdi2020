#define LOCAL_TIMER_CONTROL_REG       ((volatile unsigned int*)0x40000034)
#define LOCAL_TIMER_IRQ_CLR           ((volatile unsigned int*)0x40000038)
unsigned int CORE0_TIMER_IRQ_CTRL = 0x40000040;
unsigned int EXPIRE_PERIOD = 0xfffffff;

void core_timer_enable(){
    // can't use x0 because CORE0_TIMER_IRQ_CTRL is assigned to it by the compiler
    asm volatile (
                  // enable core timer
                  "mov x1, 1;"
                  "msr cntp_ctl_el0, x1;"
                  "mov x1, 2;"

                  // enable timer interrupt 
                  "mov x1, 2;"
                  "mov x2, %0;"
                  "str x1, [x2]"
                  ::"r"(CORE0_TIMER_IRQ_CTRL)
                 );
}

void set_core_timer_period(){
    asm volatile (
                   "mov x0, %0;"
                   "msr cntp_tval_el0, x0"
                   ::"r"(EXPIRE_PERIOD)
                 );
}

void local_timer_init(){
    unsigned int flag = 0x30000000; // enable timer and interrupt.
    unsigned int reload = 25000000;
    *LOCAL_TIMER_CONTROL_REG = flag | reload;
}

void set_local_timer_control(){
    *LOCAL_TIMER_IRQ_CLR = 0xc0000000; // clear interrupt and reload.
}