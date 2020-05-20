#ifndef __USRLIB_H__
#define __USRLIB_H__

#include "syscall.h"
#define fork call_sys_fork
#define read call_sys_read
#define exit call_sys_exit
#define write call_sys_write
#define get_taskid call_sys_task_id
#define remain_page_num call_sys_page_num
#define mmap call_sys_mmap
#define NULL ((void*)0)

#endif
