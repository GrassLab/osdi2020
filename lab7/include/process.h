#ifndef __PROCESS_H__
#define __PROCESS_H__

void kernel_process();
void task_buddy_aloc(int ret);
void task_fixed_aloc(int ret);
void task_varied_aloc(int ret);

void task_vnode_op(int ret);
void task_file_op(int ret);
void task_read_dir(int ret);

#endif
