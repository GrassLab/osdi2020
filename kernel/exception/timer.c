#include "timer.h"

void core_timer_enable ()
{
    // enable timer
    // CNTP_CTL_EL0: Control register for the EL1 physical timer.
    // bit[0]: ENABLE
    // bit[1]: IMASK
    // bit[2]: ISTATUS
    asm volatile (
        "mov x0, #1;"
        "msr CNTP_CTL_EL0, x0;"
    );
    
    // Core Timer Interrupt
    // bit[0]: nCNTPSIRQ:    secure physical timer
    // bit[1]: nCNTPNSIRQ:   Non-secure physical timer
    // bit[2]: nCNTHPIRQ:    Hypervisor Timer Event
    // bit[3]: nCNTVIRQ:     Virtual Timer Event
    // bit[4]: nCNTPSIRQ:    FIQ
    // bit[5]: nCNTPNSIRQ:   FIQ
    // bit[6]: nCNTHPIRQ:    FIQ
    // bit[7]: nCNTVIRQ:     FIQ

    *CORE0_TIMER_IRQ_CTRL = 0b00000010;
}

void core_timer_reload ()
{
    // CNTP_TVAL_EL0: Holds the timer value for the EL1 physical timer.
    // TimerValue, bits [31:0]
    asm volatile (
        "mov x0, %0;"
        "msr CNTP_TVAL_EL0, x0;"
        :
        : "r" (0xFFFFFFF)
    );
}

void local_timer_enable ()
{
    // bit[31]: INTR Flag
    // bit[30]: unused
    // bit[29]: INTR Enable
    // bit[28]: Timer Enable
    // bit[0:27]: Re-load value
    *LOCAL_TIMER_CTRL = 0b00110101000000000000000000000000;
}

void local_timer_reload()
{
    // bit[31]: INTR Flag clear
    // bit[30]: Local timer-reloaded
    // bit[0:29]: unused
    *LOCAL_TIMER_CLR = 0b11000000000000000000000000000000;
}