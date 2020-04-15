#include "uart.h"
#define LOCAL_TIMER_CONTROL_REG ((volatile unsigned int*)0x40000034)
#define LOCAL_TIMER_IRQ_CLR ((volatile unsigned int*)0x40000038)
#define SYSTEM_TIMER_COMPARE1 ((volatile unsigned int*)0x3f003010)
#define SYSTEM_TIMER_CLO ((volatile unsigned int*)0x3f003004)
#define SYSTEM_TIMER_CS ((volatile unsigned int*)0x3f003000)
#define IRQ_ENABLE0 ((volatile unsigned int*)0x3f00b210)
#define ARM_TIMER_LOAD ((volatile unsigned int*)0x3f00b400)
#define ARM_TIMER_CONTROL ((volatile unsigned int*)0x3f00b408)
#define ARM_TIMER_IRQ_CLR ((volatile unsigned int*)0x3f00b40c)
#define IRQ_ENABLE2 ((volatile unsigned int*)0x3f00b218)
#define CORE0_TIMER_IRQ_CTRL ((volatile unsigned int*)0x40000040)
#define CORE0_IRQ_SRC ((volatile unsigned int*)0x40000060)
#define EXPIRE_PERIOD ((volatile unsigned int*)0x0ffffff)
#define IRQ_BASIC_PENDING ((volatile unsigned int*)(MMIO_BASE + 0xb200))
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)0x3F10001c)
#define PM_WDOG ((volatile unsigned int*)0x3F100024)

#define set(a, b) (*a = b)
#define get(a, b) (b = *a)


void _core_timer_enable();
void reset();


void sync_el1_exc_handler(unsigned long x0, unsigned long x1, unsigned long x2, unsigned long x3)
{
    //x0 = type, x1 = par1, x2 = par2 ...
    unsigned int esr, elr, currentEL, currentSP;
    asm volatile("mrs %0,esr_el1":"=r"(esr));
    asm volatile("mrs %0,elr_el1":"=r"(elr));
    unsigned char exc_class = esr>>26;
    unsigned int ISS_bit = esr&0x1FFFFFF;

    if(exc_class == 0x3C) // brk instruction require+4
    {    asm volatile("msr elr_el1, %0"::"r"(elr+4):);}
    else if(exc_class == 0x15) // if svc call
    {
        if(ISS_bit == 0)
        {
            if(x0 == 0) //core timer enable
            {
                //uart_puts("core timer interrupt on \r\n");
                _core_timer_enable();
            }
            else if(x0 == 1) //show time stamp
            {
                volatile unsigned long long int time_FRQ, time_CT;
                asm volatile("mrs %0, CNTFRQ_EL0" : "=r"(time_FRQ) ::);
                asm volatile("mrs %0, cntpct_el0" : "=r"(time_CT)::);
                *(unsigned long long int*)x1 = time_FRQ;
                *(unsigned long long int*)x2 = time_CT;
            }
            else if(x0 == 2) //reboot
            {
                reset(1000);
            }
            else
            {
                uart_puts("unknown svc 0 call type\r\n");
            }
        }
        else if(ISS_bit == 1) //show svc info 
        {
            asm volatile("mrs %0,CurrentEL":"=r"(currentEL));
            asm volatile("mov %0,sp":"=r"(currentSP));
            *((unsigned int*)x1) = (unsigned int)exc_class;
            *((unsigned int*)x1 + 1) = esr&0x1FFFFFF;
            *((unsigned int*)x1 + 2) = elr;
            *((unsigned int*)x1 + 3) = currentEL;
            *((unsigned int*)x1 + 4) = currentSP;
        }
        else
        {
            uart_puts("other svc call type no implement\r\n");
        }
    }
    else
    {
        uart_puts("unknown sync exception\r\n");
    }
}

void irq_hanlder()
{
    static unsigned int core_count = 0, local_count = 0;
    unsigned int c0_source = *CORE0_IRQ_SRC;
    //uart_hex(c0_source);
    //uart_puts("\r\n");

    if(c0_source & 0x00000800) //Local timer interrupt handler p.16
    {
        set(LOCAL_TIMER_IRQ_CLR, 0xc0000000);
        local_count++;
        uart_puts("\r\nLocal timer interrupt: ");
        uart_hex(local_count);
    }
    else if(c0_source & 0x00000002)  // core timer handler (CNTPNSIRQ interrupt)
    {
        asm volatile("msr cntp_tval_el0, %0"::"r"(EXPIRE_PERIOD):);
        core_count++;
        uart_puts("\r\nCore timer interrupt: ");
        uart_hex(core_count);
    }
    else if(c0_source & 0x00000100) //GPU interrupt ///*IRQ_BASIC_PENDING & 0x80000
    {
        if(*UART0_RIS & 0x10) //bit 4 RX interrupt status
        {
            char c;
            while(*UART0_FR&0x40)
            {
                c = (char) (*UART0_DR);
                rec_buf.buf[rec_buf.tail++] = c;
            }
            /* read it and return */
            *UART0_ICR = 1 << 4;
            
        }
        else if(*UART0_RIS & 0x20) //bit 5 TX interrupt status  FR in p.181
        {
            //static int count = 0;
            //count++;  
            //do{asm volatile("nop");}while(*UART0_FR&0x20);
            do{asm volatile("nop");}while(!(*UART0_FR&0x80));//p.181 FR register
            //*UART0_DR='a';
            *UART0_ICR = 2 << 4;
            if( (tran_buf.tail - tran_buf.head) > 0)
            {
                *UART0_DR=tran_buf.buf[tran_buf.head++];
            }
            else
            {
                tran_buf.tail=0;
                tran_buf.head=0;
            }
        }
    }
    else
    {
        uart_puts("unknown irq interrupt\r\n");
    }
}

void local_timer_init(){
    unsigned int flag = 0x30000000; // enable timer and interrupt.
    unsigned int reload = 25000000;
    *LOCAL_TIMER_CONTROL_REG = (flag | reload);
}

void sys_timer_init(){
    unsigned int t;
    t = *SYSTEM_TIMER_CLO;
    *SYSTEM_TIMER_COMPARE1 = t + 2500000;
    *IRQ_ENABLE0 = 1 << 1;
}

void arm_timer_init(){
  set(ARM_TIMER_CONTROL, (1 << 7) | (1 << 5) | (1 << 1));
  set(ARM_TIMER_LOAD, 500000);
  set(IRQ_ENABLE2, 1);
}

void other_handler()
{
    unsigned int abc;
    uart_puts("oops! unknown exception\r\n");
    asm volatile("mrs %0, SPSel":"=r"(abc)::);
    uart_puts("sps: ");
    uart_hex(abc);
    uart_puts("\r\n");
}

void _core_timer_enable()
{
    asm volatile("msr cntp_tval_el0, %0"::"r"(EXPIRE_PERIOD):);
    asm volatile("msr cntp_ctl_el0, %0"::"r"(1):);
    set(CORE0_TIMER_IRQ_CTRL, 2);
}

void reset(int tick){ // reboot after watchdog timer expire
	*PM_RSTC = (PM_PASSWORD | 0x20);
	*PM_WDOG = (PM_PASSWORD | tick);
	/*set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
	set(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick*/
}

void cancel_reset() {
	*PM_RSTC = (PM_PASSWORD | 0);
	*PM_WDOG = (PM_PASSWORD | 0);
	/*set(PM_RSTC, PM_PASSWORD | 0); // full reset
	set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick*/
}




//b *0x81e24