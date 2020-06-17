#include "syscall.h"

.globl call_sys_read
call_sys_read:
    mov w8, #SYSNUM_READ
    svc #0
    ret

.globl call_sys_write
call_sys_write:
    mov w8, #SYSNUM_WRITE
    svc #0
    ret

.globl call_sys_exec
call_sys_exec:
    mov w8, #SYSNUM_EXEC
    svc #0
    ret

.globl call_sys_exit
call_sys_exit:
    mov w8, #SYSNUM_EXIT
    svc #0
    ret

.globl call_sys_fork
call_sys_fork:
    mov w8, #SYSNUM_FORK
    svc #0
    ret

.globl call_sys_signal
call_sys_signal:
    mov w8, #SYSNUM_SIGNAL
    svc #0
    ret

.globl call_sys_mutex_lock
call_sys_mutex_lock:
    mov w8, #SYSNUM_MUTEX_LOCK
    svc #0
    ret

.globl call_sys_mutex_unlock
call_sys_mutex_unlock:
    mov w8, #SYSNUM_MUTEX_UNLOCK
    svc #0
    ret

.globl call_sys_task_id
call_sys_task_id:
    mov w8, #SYSNUM_TASK_ID
    svc #0
    ret

.globl call_sys_page_num
call_sys_page_num:
    mov w8, #SYSNUM_PAGE_NUM
    svc #0
    ret

.globl call_sys_mmap
call_sys_mmap:
    mov w8, #SYSNUM_MMAP
    svc #0
    ret

.globl call_sys_pages
call_sys_pages:
    mov w8, #SYSNUM_PAGES
    svc #0
    ret

.globl call_sys_fopen
call_sys_fopen:
    mov w8, #SYSNUM_FOPEN
    svc #0
    ret

.globl call_sys_fclose
call_sys_fclose:
    mov w8, #SYSNUM_FCLOSE
    svc #0
    ret

.globl call_sys_fread
call_sys_fread:
    mov w8, #SYSNUM_FREAD
    svc #0
    ret

.globl call_sys_fwrite
call_sys_fwrite:
    mov w8, #SYSNUM_FWRITE
    svc #0
    ret
