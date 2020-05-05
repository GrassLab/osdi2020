#include "uart.h"
#include "task.h"


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
#define EXPIRE_PERIOD ((volatile unsigned int*)0x005ffff)
#define IRQ_BASIC_PENDING ((volatile unsigned int*)(MMIO_BASE + 0xb200))
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)0x3F10001c)
#define PM_WDOG ((volatile unsigned int*)0x3F100024)



#define set(a, b) (*a = b)
#define get(a, b) (b = *a)


void _core_timer_enable();
void reset();
void _local_timer_handler();
void set_trap_ret(unsigned long long ret, int id);
int sys_get_taskid();


void sync_el1_exc_handler(unsigned long long x0, unsigned long long x1, unsigned long long x2, unsigned long long x3)
{
    //unsigned long long *sp;
    //asm volatile("mov %0, x7":"=r"(sp)::);
    //x0 = type, x1 = par1, x2 = par2 ...
    unsigned long long esr, currentEL, currentSP, sp_el0, elr_el1, spsr_el1;
    asm volatile("mrs %0,esr_el1":"=r"(esr));
    asm volatile("mrs %0,elr_el1":"=r"(elr_el1));
    asm volatile("mrs %0,sp_el0":"=r"(sp_el0));
    asm volatile("mrs %0,spsr_el1":"=r"(spsr_el1));
    unsigned char exc_class = esr>>26;
    unsigned int ISS_bit = esr&0x1FFFFFF;
    if(exc_class == 0x3C) // brk instruction require+4
    {    
        asm volatile("msr elr_el1, %0"::"r"(elr_el1+4):);
    }
    else if(exc_class == 0x15) // if svc call
    {
        if(ISS_bit == 0)
        {
            if(x0 == 0) //core timer enable
            {
                uart_puts("core timer interrupt on \r\n");
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
            else if(x0 == 3) // schedule
            {
                //uart_puts("syscall...\r\n");
                //asm volatile("msr daifclr, 0xf");
                task_schedule();
            }
            else if(x0 == 4) //get task id
            {
                set_trap_ret((unsigned long long)sys_get_taskid(), get_current_task()->id);
            }
            else if(x0 == 5) //uart read
            {
                set_trap_ret((unsigned long long)uart_getc(), get_current_task()->id);
            }
            else if(x0 == 6) // uart write
            {
                uart_puts((char*)x1);
            }
            else if(x0 == 7)
            {
                do_exec((void *)x1, 1);
            }
            else if(x0 == 8)
            {
                task *currentsk = get_current_task();
                int id = sys_do_fork();
                unsigned long long parent_x29 = currentsk->trapframe[29];
                unsigned long long parent_ubase = currentsk->ubase;

                task_pool[id].trapframe = task_pool[id].kbase - (currentsk->kbase - (unsigned long long)currentsk->trapframe);
                
                
                fork_ret(&task_pool[id], currentsk->kbase, task_pool[id].kbase);
                currentsk = get_current_task();
                //my_printf("fork now id: %d\r\n", currentsk->id);
                if(currentsk->id != id)
                {
                    //uart_hex(currentsk->trapframe[29]);
                    set_trap_ret(id, currentsk->id);
                }
                else{
                    currentsk->trapframe[29] = currentsk->ubase - (parent_ubase - parent_x29);  //rebase user stack x29
                    //uart_hex(currentsk->trapframe[29]);
                    set_trap_ret(0, id);
                }
                //uart_hex( task_pool[id].trapframe[0] );
                
               
            }
            else if(x0 == 9)
            {
                //x1 = exit value
                runqueue_del(get_current_task()->id);
                asm volatile("msr daifclr, 0xf");
                task_schedule();
            }
            else if(x0 == 10)
            {
                uart_hex(x1);
            }
            else if(x0 == 11)
            {
                uart_send((char)x1);
            }
            else
            {
                uart_puts("unknown svc 0 call type\r\n");
            }
        }
        else if(ISS_bit == 1) //show svc info 
        {
            asm volatile("msr daifclr, 0x2");
            asm volatile("mrs %0,daif":"=r"(currentEL));
            asm volatile("mov %0,sp":"=r"(currentSP));

            *((unsigned long long*)x1) = (unsigned long long)exc_class;
            *((unsigned long long*)x1 + 1) = esr&0x1FFFFFF;
            *((unsigned long long*)x1 + 2) = elr_el1;
            *((unsigned long long*)x1 + 3) = currentEL;
            *((unsigned long long*)x1 + 4) = currentSP;
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
    /*unsigned long long tmp2;
    asm volatile("mov %0, sp":"=r"(tmp2)::);
    asm volatile("mov sp, %0"::"r"(0x100000):);*/

    static unsigned long long core_count = 0, local_count = 0;;
    unsigned int c0_source = *CORE0_IRQ_SRC;
    unsigned long long tmp;

    //uart_hex(c0_source);
    //uart_puts("irq\r\n");

    if(c0_source & 0x00000800) //Local timer interrupt handler p.16
    {
        set(LOCAL_TIMER_IRQ_CLR, 0xc0000000);
        local_count++;
        uart_puts("\r\nLocal timer interrupt: ");
        uart_hex(local_count);
    }
    else if(c0_source & 0x00000002)  // core timer handler (CNTPNSIRQ interrupt)
    { 
        task *current_task = get_current_task();
        core_count++;
        
        _global_coretimer = core_count;
        asm volatile("msr cntp_tval_el0, %0"::"r"(EXPIRE_PERIOD):);
        if( ((core_count - current_task->start_coretime) > 2) || ((core_count - current_task->start_coretime) < 0 ) ){
            //uart_puts("schedule\r\n");
            task_schedule();
        }
        
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
            //if( *UART0_ICR & (2<<4))
            //*TMP_SOR = *TMP_SOR+1;

            /*------------------*/
            if( (tran_buf.tail - tran_buf.head) > 0)
            {
                *UART0_DR=tran_buf.buf[tran_buf.head++];
            }
            else
            {
                tran_buf.tail=0;
                tran_buf.head=0;
            }
            *UART0_ICR = 2 << 4;
        }
    }
    else
    {
        uart_puts("unknown irq interrupt\r\n");
    }

    //asm volatile("mov sp, %0"::"r"(tmp2):);
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

void _local_timer_handler()
{
    static unsigned int local_count = 0;
    unsigned int size = 2147483647; 
    while(size--)
        asm volatile("nop");
    set(LOCAL_TIMER_IRQ_CLR, 0xc0000000);
    local_count++;
    uart_puts("\r\nLocal timer interrupt: ");
    uart_hex(local_count);
}

void set_trap_ret(unsigned long long ret, int id)
{
    //task *current = get_current_task();
    task_pool[id].trapframe[0] = ret;
}

int sys_get_taskid()
{
    task *current = get_current_task();
    return current->id;
}

int sys_do_fork()
{
    //extern char kstack_pool[64][4096];
    task *current = get_current_task();
    int new_id = privilege_task_create((void *)current->fp_lr[1]); //will not use this ret address
    task_pool[new_id].privilege = current->privilege;
    task_pool[new_id].elr_el1 = current->elr_el1;
    task_pool[new_id].spsr_el1 = current->spsr_el1;
    unsigned long long sgap = (current->ubase - current->sp_el0);
    task_pool[new_id].sp_el0 -= sgap;
    
    //my_printf("current base stack: %x, usp: %x\r\n", user_pool[current->id], current->sp_el0);
    

    for(int i=0;i<sgap;i++)
    {
        ((char*)(task_pool[new_id].sp_el0))[i] = ((char*)(current->sp_el0))[i];
    }

    //my_printf("child base stack: %x, usp: %x\r\n", user_pool[new_id], task_pool[new_id].sp_el0);

    //while(1);
    
    return new_id;
}
//b *0x81e24

