// #include "../include/gpio.h"
// #include "../include/mailbox.h"
// #include "../include/utils.h"
// #include "../include/peripherals/timer.h"


#include "../include/uart.h"

#define set(a, b) *a = b

extern unsigned int CORE_TIMER_COUNT;
extern unsigned int LOCAL_TIMER_COUNT;

void debug(){
    uart_puts("debugggggg!!\n");
}

void disable_irq() 
{
    asm volatile("msr daifset, 0xf");
}

void enable_irq() 
{
    asm volatile("msr daifclr, 0xf");
}

void set_HCR_EL2_IMO()
{
    //asm volatile("mov x0, #(1 << 4)");
    //asm volatile("msr hcr_el2, x0");
    asm volatile("mrs x0, hcr_el2");
    asm volatile("orr x0, x0, #16");
    asm volatile("msr hcr_el2, x0");
}

void core_timer_counter()
{
    uart_puts("Core Timer interrupt received ");
    uart_hex(CORE_TIMER_COUNT++);
    uart_puts("\n");
}

#define CORE0_TIMER_IRQ_CTRL (unsigned int* )0x40000040
void core_timer_enable()
{
    
    asm volatile("mrs x0, cntfrq_el0");
    asm volatile("msr cntp_tval_el0, x0");
    asm volatile("mov x0, 1");
	asm volatile("msr cntp_ctl_el0, x0");
    set(CORE0_TIMER_IRQ_CTRL, 0x2);

    //asm volatile ("mov x0, #0\n" "svc #0\n");
    
    //unsigned int val = EXPIRE_PERIOD;
    //asm volatile("msr cntp_tval_el0, %0" :: "r" (val));
}

void core_timer_enable_user()
{
    asm volatile("mov x0, #0\n" "svc #0\n");
}

#define EXPIRE_PERIOD 0xffffff
void core_timer_handler()
{
    // unsigned int CNTP_CVAL_EL0;
    // asm volatile("mrs %0, cntp_cval_el0" : "=r"(CNTP_CVAL_EL0));
    // uart_puts("CNTP_CVAL_EL0: ");
    // uart_hex(CNTP_CVAL_EL0);
    // uart_puts("\n");

    asm volatile("mrs x0, cntfrq_el0");
    asm volatile("msr cntp_tval_el0, x0");

    //unsigned int val = EXPIRE_PERIOD;
    //asm volatile("msr cntp_tval_el0, %0" :: "r" (val));
    core_timer_counter();
}


#define LOCAL_TIMER_CONTROL_REG (unsigned int* )0x40000034

void local_timer_init()
{
    unsigned int flag = 0x30000000; // enable timer and interrupt.
    unsigned int reload = 25000000;
    set(LOCAL_TIMER_CONTROL_REG, (flag | reload));
}

void local_timer_counter()
{
    uart_puts("Local Timer interrupt received ");
    uart_hex(LOCAL_TIMER_COUNT++);
    uart_puts("\n");
}

#define LOCAL_TIMER_IRQ_CLR (unsigned int* )0x40000038
#define LOCAL_TIMER_RELOAD (unsigned int* )0xc0000000//0xc0000000
void local_timer_handler()
{
    set(LOCAL_TIMER_IRQ_CLR, LOCAL_TIMER_RELOAD); // clear interrupt and reload.
    local_timer_counter();
}


#define CORE0_INTR_SRC (unsigned int* )0x40000060
void interrupt_handler()
{
    unsigned int arm_local = *CORE0_INTR_SRC;
    // local timer interrupt
    if (arm_local & 0x800) {
        local_timer_handler();
    }
    // core timer interrupt
    else if (arm_local & 0x2) {
        core_timer_handler();
    }
    else {
        uart_puts("interrupt_handler error.");
    }
}




// #define MBOX_TAG_GET_CLOCK_RATE	0x00030002

// #define ARMTIMER_CONTROL ((volatile unsigned int*)(MMIO_BASE+0xB408))
// #define ARMTIMER_LOAD    ((volatile unsigned int*)(MMIO_BASE+0xB400))
// #define IRQ_ENABLE_BASIC_IRQS    ((volatile unsigned int*)(MMIO_BASE+0xB218))
// #define RPI_BASIC_ARM_TIMER_IRQ         ( 1 << 0 )

// #define RPI_ARMTIMER_CTRL_23BIT         ( 1 << 1 )
// #define RPI_ARMTIMER_CTRL_PRESCALE_1    ( 0 << 2 )
// #define RPI_ARMTIMER_CTRL_IRQ_ENABLE    ( 1 << 5 )
// #define RPI_ARMTIMER_CTRL_ENABLE        ( 1 << 7 )




// void timer_irq_setup() 
// {
//     unsigned int divisor;
//     // Make sure clock is stopped, illegal to change anything while running
//     // #define ARMTIMER_CONTROL ((volatile unsigned int*)(MMIO_BASE+0xB408))
//     // #define RPI_ARMTIMER_CTRL_ENABLE        ( 1 << 7 )
//     *ARMTIMER_CONTROL &= ~(RPI_ARMTIMER_CTRL_ENABLE); 
//     // Get GPU clock (it varies between 200-450Mhz)
//     mbox[0] = (5 + 3) * 4;          // length of the message
//     mbox[1] = MBOX_REQUEST;         // this is a request message
    
//     mbox[2] = MBOX_TAG_GET_CLOCK_RATE;   // #define MBOX_TAG_GET_CLOCK_RATE	0x00030002
//     mbox[3] = 8;                    // buffer size
//     mbox[4] = 8;
//     mbox[5] = 4;                    // CLOCK ID: CORE
//     mbox[6] = 0;                    // Clock Frequency

//     mbox[7] = MBOX_TAG_LAST;

//     // send the message to the GPU and receive answer
//     if (mbox_call(MBOX_CH_PROP)) {
        
//         //printf("\nclock freq: %d Hz\n", mbox[6]);
            
//         // The prescaler divider is set to 250 (based on GPU=250MHz to give 1Mhz clock)
//         mbox[6] /= 250;												
//         // Divisor we would need at current clock speed
//         //divisor = ((unsigned long int)period_in_us * mbox[6]) / 1000000;			
//         divisor = 500000;
//         //printf("\nclock divisor: %d\n", divisor);
        
//         // Enable the timer interrupt IRQ
//         // #define IRQ_ENABLE_BASIC_IRQS    ((volatile unsigned int*)(MMIO_BASE+0xB218))
//         // #define RPI_BASIC_ARM_TIMER_IRQ         (1 << 0)
//         *IRQ_ENABLE_BASIC_IRQS |=  RPI_BASIC_ARM_TIMER_IRQ;
//         // Set the load value to divisor
//         // #define ARMTIMER_LOAD    ((volatile unsigned int*)(MMIO_BASE+0xB400))
//         *ARMTIMER_LOAD = divisor;	

//         // #define ARMTIMER_CONTROL ((volatile unsigned int*)(MMIO_BASE+0xB408))
//         // #define RPI_ARMTIMER_CTRL_23BIT         ( 1 << 1 )
//         // #define RPI_ARMTIMER_CTRL_PRESCALE_1     ( 0 << 2 )
//         // #define RPI_ARMTIMER_CTRL_IRQ_ENABLE    ( 1 << 5 )
//         // #define RPI_ARMTIMER_CTRL_ENABLE        ( 1 << 7 )
//         *ARMTIMER_CONTROL |= RPI_ARMTIMER_CTRL_23BIT | RPI_ARMTIMER_CTRL_PRESCALE_1 | RPI_ARMTIMER_CTRL_IRQ_ENABLE | RPI_ARMTIMER_CTRL_ENABLE; 
//         //printf("irq_setup finished. Control: %x", *ARMTIMER_CONTROL);
//     }
// }