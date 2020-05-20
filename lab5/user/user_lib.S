.section ".text.entry"

// entry point (the start address of .text.entry in linker.ld)
.global _user_entry
_user_entry:
    bl main
    //bl exit_handler
    bl user_exit

.set SYS_WRITE_NUMBER,        0        // syscal numbers
.set SYS_FORK_NUMBER,         1
.set SYS_EXIT_NUMBER,         2
.set SYS_READ_NUMBER,         3
.set SYS_STATUS_MEM_NUMBER,   4
.set SYS_TASKID_NUMBER,       5
.set SYS_REMAIN_PAGE_NUMBER,  6

.globl user_delay
user_delay:
    subs x0, x0, #1
    bne user_delay
    ret

.global user_write
user_write:
    mov w8, #SYS_WRITE_NUMBER
    svc #0
    ret

.globl user_exit
user_exit:
    mov w8, #SYS_EXIT_NUMBER
    svc #0
    ret

.globl user_fork
user_fork:
    mov w8, #SYS_FORK_NUMBER
    svc #0
    ret

.globl user_read
user_read:
    mov w8, #SYS_READ_NUMBER
    svc #0
    ret

.globl user_mem_status
user_mem_status:
    mov w8, #SYS_STATUS_MEM_NUMBER
    svc #0
    ret

.globl get_taskid
get_taskid:
    mov w8, #SYS_TASKID_NUMBER
    svc #0
    ret


.globl remain_page_num
remain_page_num:
    mov w8, #SYS_REMAIN_PAGE_NUMBER
    svc #0
    ret
