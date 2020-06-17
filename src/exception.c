#include "peripherals/irq.h"
#include "peripherals/uart0.h"
#include "peripherals/arm.h"
#include "timer.h"
#include "uart0.h"
#include "exception.h"
#include "schedule.h"
#include "sys.h"
#include "mm.h"
#include "vfs.h"

char *intr_stack;
uint64_t arm_core_timer_jiffies = 0, arm_local_timer_jiffies = 0;
uint64_t cntfrq_el0, cntpct_el0;

void irq_enable() {
    asm volatile("msr daifclr, #2");
}

void irq_disable() {
    asm volatile("msr daifset, #2");
}

void exc_init() {
    intr_stack = (char*) kmalloc(INTR_STK_SIZE);
}

/*
 * Synchronous Exception
 */

void sys_get_task_id(struct trapframe* trapframe) {
    uint64_t task_id = get_current_task()->id;
    trapframe->x[0] = task_id;
}

void sys_uart_read(struct trapframe* trapframe) {
    char* buf = (char*) trapframe->x[0];
    uint32_t size = trapframe->x[1];

    irq_enable();
    for (uint32_t i = 0; i < size; i++) {
        buf[i] = uart0_read();
    }
    buf[size] = '\0';
    irq_disable();
    trapframe->x[0] = size;
}

void sys_uart_write(struct trapframe* trapframe) {
    const char* buf = (char*) trapframe->x[0];
    uint32_t size = trapframe->x[1];

    uart_printf("%s", buf);
    trapframe->x[0] = size;
}

// void sys_exec(struct trapframe* trapframe) {
//     void (*func)() = (void(*)()) trapframe->x[0];
//     do_exec(func);
//     trapframe->x[0] = 0;
// }

void sys_fork(struct trapframe* trapframe) {
    struct task_t* parent_task = get_current_task();

    int child_id = privilege_task_create(return_from_fork, parent_task->priority);
    struct task_t* child_task = &task_pool[child_id];

    // copy kernel stack
    char* child_kstack = &kstack_pool[child_task->id][KSTACK_TOP_IDX];
    char* parent_kstack = &kstack_pool[parent_task->id][KSTACK_TOP_IDX];
    uint64_t kstack_offset = parent_kstack - (char*)trapframe;
    for (uint64_t i = 0; i < kstack_offset; i++) {
        *(child_kstack - i) = *(parent_kstack - i);
    }
    // place child's kernel stack to right place
    child_task->cpu_context.sp = (uint64_t)child_kstack - kstack_offset;

    // copy all user pages
    fork_pgd(current_task, child_task);

    // place child's user stack to right place
    struct trapframe* child_trapframe = (struct trapframe*) child_task->cpu_context.sp;
    child_trapframe->sp_el0 = trapframe->sp_el0;

    child_trapframe->x[0] = 0;
    trapframe->x[0] = child_task->id;
}

void sys_exit(struct trapframe* trapframe) {
    do_exit(trapframe->x[0]);
}

void sys_remain_page(struct trapframe* trapframe) {
    int remain_page = 0;
    for (int i = 0; i < MAX_BUDDY_ORDER; i++) {
        remain_page += free_area[i].nr_free * (1 << i);
    }
    trapframe->x[0] = remain_page;
}

void sys_open(struct trapframe* trapframe) {
    const char* pathname = (char*) trapframe->x[0];
    int flags = trapframe->x[1];
    struct file* f = vfs_open(pathname, flags);
    // open failed
    if (f == NULL) {
        trapframe->x[0] = -1;
        return;
    }
    // store fd in task struct
    int fd_num = current_task->files.next_fd;
    // open files more than fd array
    if (fd_num >= current_task->files.count) {
        int new_fd_array_size = current_task->files.count + NR_OPEN_DEFAULT;
        struct file** new_fd_array = (struct file**)kmalloc(sizeof(struct file*) * new_fd_array_size);
        for (int i = 0; i < current_task->files.count; i++) {
            new_fd_array[i] = current_task->files.fd[i];
        }
        current_task->files.fd = new_fd_array;
        current_task->files.count = new_fd_array_size;
    }
    current_task->files.fd[fd_num] = f;
    current_task->files.next_fd++;
    trapframe->x[0] = fd_num;
}

void sys_close(struct trapframe* trapframe) {
    int fd_num = (int)trapframe->x[0];
    if (fd_num < 0) {
        trapframe->x[0] = -1;
        return;
    }
    struct file* f = current_task->files.fd[fd_num];
    trapframe->x[0] = vfs_close(f);
    current_task->files.fd[fd_num] = NULL;
}

void sys_write(struct trapframe* trapframe) {
    int fd_num = (int)trapframe->x[0];
    char* buf = (char*)trapframe->x[1];
    if (fd_num < 0) {
        trapframe->x[0] = -1;
        return;
    }
    uint64_t len = (uint64_t)trapframe->x[2];
    struct file* f = current_task->files.fd[fd_num];
    if (f == NULL) {
        trapframe->x[0] = 0;
        return;
    }
    trapframe->x[0] = vfs_write(f, buf, len);
}

void sys_read(struct trapframe* trapframe) {
    int fd_num = (int)trapframe->x[0];
    char* buf = (char*)trapframe->x[1];
    if (fd_num < 0) {
        trapframe->x[0] = -1;
        return;
    }
    uint64_t len = (uint64_t)trapframe->x[2];
    struct file* f = current_task->files.fd[fd_num];
    if (f == NULL) {
        trapframe->x[0] = 0;
        return;
    }
    trapframe->x[0] = vfs_read(f, buf, len);
}

void sys_readdir(struct trapframe* trapframe) {
    int fd_num = (int)trapframe->x[0];
    if (fd_num < 0) {
        trapframe->x[0] = -1;
        return;
    }
    struct file* f = current_task->files.fd[fd_num];
    trapframe->x[0] = vfs_readdir(f);
}

void sys_mkdir(struct trapframe* trapframe) {
    const char* pathname = (char*) trapframe->x[0];
    trapframe->x[0] = vfs_mkdir(pathname);
}

void sys_chdir(struct trapframe* trapframe) {
    const char* pathname = (char*) trapframe->x[0];
    trapframe->x[0] = vfs_chdir(pathname);
}

void sys_mount(struct trapframe* trapframe) {
    const char* device = (char*) trapframe->x[0];
    const char* mountpoint = (char*) trapframe->x[1];
    const char* filesystem = (char*) trapframe->x[2];
    trapframe->x[0] = vfs_mount(device, mountpoint, filesystem);
}

void sys_umount(struct trapframe* trapframe) {
    const char* mountpoint = (char*) trapframe->x[0];
    trapframe->x[0] = vfs_umount(mountpoint);
}

void sys_call_router(uint64_t sys_call_num, struct trapframe* trapframe) {
    switch (sys_call_num) {
        case SYS_GET_TASK_ID:
            sys_get_task_id(trapframe);
            break;

        case SYS_UART_READ:
            sys_uart_read(trapframe);
            break;

        case SYS_UART_WRITE:
            sys_uart_write(trapframe);
            break;

        // case SYS_EXEC:
        //     sys_exec(trapframe);
        //     break;

        case SYS_FORK:
            sys_fork(trapframe);
            break;

        case SYS_EXIT:
            sys_exit(trapframe);
            break;

        case SYS_REMAIN_PAGE:
            sys_remain_page(trapframe);
            break;

        case SYS_OPEN:
            sys_open(trapframe);
            break;

        case SYS_CLOSE:
            sys_close(trapframe);
            break;

        case SYS_WRITE:
            sys_write(trapframe);
            break;

        case SYS_READ:
            sys_read(trapframe);
            break;

        case SYS_READDIR:
            sys_readdir(trapframe);
            break;

        case SYS_MKDIR:
            sys_mkdir(trapframe);
            break;

        case SYS_CHDIR:
            sys_chdir(trapframe);
            break;

        case SYS_MOUNT:
            sys_mount(trapframe);
            break;

        case SYS_UMOUNT:
            sys_umount(trapframe);
            break;
    }
}

void page_fault_handler() {
    register uint64_t fault_addr;
    asm volatile("mrs %0, FAR_EL1": "=r"(fault_addr));
    uart_printf("Page fault address at 0x%x, killed\n", fault_addr);
    do_exit(0);
}

void sync_exc_router(unsigned long esr, unsigned long elr, struct trapframe* trapframe) {
    // irq_enable();

    int ec = (esr >> 26) & 0b111111;
    int iss = esr & 0x1FFFFFF;
    if (ec == 0b010101) {  // system call
        uint64_t syscall_num = trapframe->x[8];
        sys_call_router(syscall_num, trapframe);
        // switch (iss) {
        //     case 1:
        //         uart_printf("Exception return address 0x%x\n", elr);
        //         uart_printf("Exception class (EC) 0x%x\n", ec);
        //         uart_printf("Instruction specific syndrome (ISS) 0x%x\n", iss);
        //         break;
        //     case 2:
        //         arm_core_timer_enable();
        //         arm_local_timer_enable();
        //         break;
        //     case 3:
        //         arm_core_timer_disable();
        //         arm_local_timer_disable();
        //         break;
        //     case 4:
        //         asm volatile ("mrs %0, cntfrq_el0" : "=r" (cntfrq_el0)); // get current counter frequency
        //         asm volatile ("mrs %0, cntpct_el0" : "=r" (cntpct_el0)); // read current counter
        //         break;
        // }
    }
    else if (ec == 0x24) {
        page_fault_handler();
    }
    else {
        uart_printf("Exception return address 0x%x\n", elr);
        uart_printf("Exception class (EC) 0x%x\n", ec);
        uart_printf("Instruction specific syndrome (ISS) 0x%x\n", iss);
    }
}

/*
 * IRQ Exception
 */

void uart_intr_handler() {
    if (*UART0_MIS & 0x10) {           // UARTTXINTR
        while (!(*UART0_FR & 0x10)) {  // RX FIFO not empty
            char r = (char)(*UART0_DR);
            uart_queue_push(&read_buf, r);
        }
        *UART0_ICR = 1 << 4;
    }
    else if (*UART0_MIS & 0x20) {           // UARTRTINTR
        while (!uart_queue_empty(&write_buf)) {  // flush buffer to TX
            while (*UART0_FR & 0x20) {      // TX FIFO is full
                asm volatile("nop");
            }
            *UART0_DR = uart_queue_pop(&write_buf);
        }
        *UART0_ICR = 2 << 4;
    }
}

void arm_core_timer_intr_handler() {
    register unsigned int expire_period = CORE_TIMER_EXPRIED_PERIOD;
    asm volatile("msr cntp_tval_el0, %0" : : "r"(expire_period));

    // check current task running time
    struct task_t *current = get_current_task();
    if (--current->counter <= 0) {
        current->counter = 0;
        current->need_resched = 1;
    }
    // irq_enable();
}

void arm_local_timer_intr_handler() {
    *LOCAL_TIMER_IRQ_CLR = 0b11 << 30;  // clear interrupt
    uart_printf("Local timer interrupt, jiffies %d\n", ++arm_local_timer_jiffies);
}

void irq_exc_router() {
    unsigned int irq_basic_pending = *IRQ_BASIC_PENDING;
    unsigned int core0_intr_src = *CORE0_INTR_SRC;

    // GPU IRQ 57: UART Interrupt
    if (irq_basic_pending & (1 << 19)) {
        uart_intr_handler();
    }
    // ARM Core Timer Interrupt
    else if (core0_intr_src & (1 << 1)) {
        arm_core_timer_intr_handler();
    }
    // ARM Local Timer Interrupt
    else if (core0_intr_src & (1 << 11)) {
        arm_local_timer_intr_handler();
    }
}

void irq_stk_switcher() {
    // Switch to interrupt stack if entry_sp in kernel stack
    register char* entry_sp;
    asm volatile("mov %0, sp": "=r"(entry_sp));
    if (!(entry_sp <= &intr_stack[4095] && entry_sp >= &intr_stack[0])) {
        asm volatile("mov sp, %0" : : "r"(&intr_stack[INTR_STK_TOP_IDX]));
    }

    irq_exc_router();

    // Restore to kernel stack if entry_sp in kernel stack
    if (!(entry_sp <= &intr_stack[4095] && entry_sp >= &intr_stack[0])) {
        asm volatile("mov sp, %0" : : "r"(entry_sp));
    }
}

void irq_return() {
    // check reschedule flag
    struct task_t *current = get_current_task();
    if (current->need_resched) {
        current->counter = TASK_EPOCH;
        current->need_resched = 0;
        schedule();
    }
}
