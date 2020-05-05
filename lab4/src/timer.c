#define LOCAL_TIMER_CONTROL_REG       ((volatile unsigned int*)0x40000034)
#define LOCAL_TIMER_IRQ_CLR           ((volatile unsigned int*)0x40000038)
unsigned int CORE0_TIMER_IRQ_CTRL = 0x40000040;
unsigned int EXPIRE_PERIOD = 0x0080000;

void core_timer_enable(){
    // can't use x0, x1 because CORE0_TIMER_IRQ_CTRL, EXPIRE_PERIOD will be assigned to them by the compiler
    // don't overwirte them
    asm volatile (
                  // can solve above situation by moving variables into register first 
                  // because the compiler won't optimize the inline asm
                  "mov x0, %0;"
                  "mov x1, %1;"

                  // enable core timer
                  "mov x2, 1;"
                  "msr cntp_ctl_el0, x2;"

                  // set expired time
                  "msr cntp_tval_el0, x0;"

                  // enable timer interrupt 
                  "mov x2, x1;"
                  "mov x1, 2;"
                  "str x1, [x2]"
                  ::"r"(EXPIRE_PERIOD), "r"(CORE0_TIMER_IRQ_CTRL)
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

void core_timer_disable(){
    asm volatile (
                  "mov x2, 0;"
                  "msr cntp_ctl_el0, x2;"
                 );
}

void local_timer_disable(){
    *LOCAL_TIMER_CONTROL_REG = 0;
}