#include "meta_macro.h"
#include "sys.h"
#include "uart.h"
#include "string_util.h"
#include "timer.h"
#include "irq.h"
#include "task.h"
#include "schedule.h"
#include "mmu.h"
#include "slab.h"

int sys_exc(uint64_t ELR_EL1, uint8_t exception_class, uint32_t exception_iss)
{
  /* obsolete */
  char string_buff[0x20];

  uart_puts("Exception return address: ");
  string_ulonglong_to_hex_char(string_buff, ELR_EL1);
  uart_puts(string_buff);
  uart_putc('\n');

  uart_puts("Exception class (EC): ");
  string_ulonglong_to_hex_char(string_buff, exception_class);
  uart_puts(string_buff);
  uart_putc('\n');

  uart_puts("Instruction specific syndrome (ISS): ");
  string_ulonglong_to_hex_char(string_buff, exception_iss);
  uart_puts(string_buff);
  uart_putc('\n');

  return 0;
}

int sys_timer_int(void)
{
  /* obsolete */
  static int core_timer_enabled = 0;
  static int local_timer_enabled = 0;

  if(!core_timer_enabled)
  {
    timer_enable_core_timer();
    core_timer_enabled = 1;
  }
  if(!local_timer_enabled)
  {
    timer_enable_and_set_local_timer();
  }
  timer_set_core_timer_sec(CORE_TIMER_SECS);

  return 0;
}

int sys_uart_puts(char * string)
{
  irq_int_enable();
  uart_puts(string);
  return 0;
}

int sys_uart_gets(char * string, char delimiter, unsigned length)
{
  irq_int_enable();
  uart_gets(string, delimiter, length);
  return 0;
}

int sys_exec_todo(void(*start_func)())
{
  UNUSED(start_func);
  //task_do_exec(start_func);
  return 0;
}

int sys_fork(struct trapframe_struct * trapframe)
{
  uint64_t current_task_id = task_get_current_task_id();

  TASK_GUARD();
  /* new task should return to exception handler context restoration */
  uint64_t new_task_id = task_privilege_task_create(__sys_fork_child_entry, (unsigned)kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].priority);

  /* create user space va */
  mmu_create_user_pmd_pte(&(kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].user_space_mm));

  /* Ask scheduler to switch pmd because it use user space */
  SET_BIT(kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].flag, TASK_STATE_USER_SPACE);

  /* new task will use context switch to restore */
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.x19 = trapframe -> x19;
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.x20 = trapframe -> x20;
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.x21 = trapframe -> x21;
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.x22 = trapframe -> x22;
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.x23 = trapframe -> x23;
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.x24 = trapframe -> x24;
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.x25 = trapframe -> x25;
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.x26 = trapframe -> x26;
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.x27 = trapframe -> x27;
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.x28 = trapframe -> x28;
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.fp = trapframe -> x29; /* wrong but let's forget it for now */
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.spsr_el1 = trapframe -> spsr_el1;
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.elr_el1 = trapframe -> elr_el1;

  /* copy fd table */
  for(int i = 0; i < TASK_MAX_FD; ++i)
  {
    kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].fd[i] = kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].fd[i];
  }

  /* copy current_dir_vnode */
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].current_dir_vnode = kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].current_dir_vnode;

  /* copy kernel stack */
/* task_kernel_stack_size is uint16_t (2 bytes) */
  memcopy((char *)(task_kernel_stack_pool + TASK_ID_TO_IDX(current_task_id) * TASK_KERNEL_STACK_SIZE), (char *)(task_kernel_stack_pool + TASK_ID_TO_IDX(new_task_id) * TASK_KERNEL_STACK_SIZE), TASK_KERNEL_STACK_SIZE * 2);

  /* setup kernel stack sp */
  /* address should be stack based - the offset of how much stack space used */
  /* pointer trapframe is current task sp */
  /* kernel sp will be used when context switch to new task */
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.sp = (uint64_t)(task_kernel_stack_pool + new_task_id * TASK_KERNEL_STACK_SIZE) - ((uint64_t)(task_kernel_stack_pool + current_task_id * TASK_KERNEL_STACK_SIZE) - (uint64_t)trapframe);

  /* setup user stack sp */
  /* should be the same as the curret one */
  kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.sp_el0 = trapframe -> sp_el0;

  /* copy user task text and stack */
  mmu_copy_user_text_stack(&(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].user_space_mm), &(kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].user_space_mm));

  /* child should get 0 on fork return */
  /* NOTE: EFFECTED BY trap dispatcher */
  /* x0 of child's trapframe is located at sp + (272 + 32) - 16 bytes */
  *(uint64_t *)((uint64_t)(kernel_task_pool[TASK_ID_TO_IDX(new_task_id)].cpu_context.sp) + 288) = 0;

  /* return value of parent should be new task id */
  *(uint64_t *)(((uint64_t)trapframe) + 288) = new_task_id;

  /* child's sp_el0 (sp + 256) and fp (ignored) in trapframe remains the same */

  TASK_UNGUARD();

  return 0;
}

int sys_exit(int status)
{
  UNUSED(status);
  uint64_t current_task_id = task_get_current_task_id();
  irq_int_enable();
  TASK_GUARD();

  SET_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, TASK_STATE_ZOMBIE);
  ++schedule_zombie_count;

  /* reclaim pages */
  mmu_reclaim_user_pages(&(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].user_space_mm));
  CLEAR_BIT(kernel_task_pool[TASK_ID_TO_IDX(current_task_id)].flag, TASK_STATE_USER_SPACE);

  TASK_UNGUARD();
  schedule_yield();
  return 0;
}

int sys_signal(int task_id, int signalno)
{
  TASK_GUARD();
  SET_BIT(kernel_task_pool[TASK_ID_TO_IDX(task_id)].signal, signalno);
  TASK_UNGUARD();
  return 0;
}

int sys_malloc(unsigned bytes)
{
  irq_int_enable();
  slab_malloc(bytes);
  return 0;
}

int sys_free(uint64_t * va)
{
  irq_int_enable();
  slab_malloc_free(va);
  return 0;
}

int sys_open(const char * pathname, int flags)
{
  struct vfs_file_struct * file = vfs_open(pathname, flags);
  if(file == 0) {
    return -1;
  }
  else
  {
    return (int)task_set_fd(file);
  }
}

int sys_close(int fd)
{
  if(fd < 0)
  {
    return fd;
  }
  vfs_close(task_get_vfs_file(fd));
  task_unset_fd(fd);
  return 0;
}

int sys_write(int fd, const void * buf, size_t len)
{
  if(fd < 0)
  {
    return fd;
  }
  return vfs_write(task_get_vfs_file(fd), buf, len);
}

int sys_read(int fd, void * buf, size_t len)
{
  if(fd < 0)
  {
    return fd;
  }
  return vfs_read(task_get_vfs_file(fd), buf, len);
}

int sys_list(int fd)
{
  if(fd < 0)
  {
    return fd;
  }
  return vfs_list(task_get_vfs_file(fd));
}

int sys_mkdir(const char * pathname)
{
  vfs_mkdir(vfs_traverse(pathname, VFS_TRAVERSE_RETURN_NEAREST), pathname);
  return 0;
}

int sys_chdir(const char * pathname)
{
  vfs_chdir(vfs_traverse(pathname, VFS_TRAVERSE_NO_RETURN_NEAREST));
  return 0;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
int sys_mount(const char * device, const char * mountpoint, const char * filesystem)
{
  /* check if the filesystem is registered */
  struct vfs_filesystem_struct * fs = vfs_get_fs(filesystem);
  if(fs == 0)
  {
    uart_puts("FS not found\n");
    return 1;
  }

  /* find mountpoint */
  struct vfs_vnode_struct * mountpoint_vnode = vfs_traverse(mountpoint, VFS_TRAVERSE_NO_RETURN_NEAREST);

  /* setup new mount */
  struct vfs_mount_struct * mount;
  vfs_setup_mount(fs, &mount);

  /* mount onto mountoint */
  vfs_mount(mountpoint_vnode, mount);

  return 0;
}

int sys_umount(const char * mountpoint)
{
  char dir_name[0x80];
  char parent_dir_name[0x80];
  string_copy(mountpoint, dir_name);
  string_copy(mountpoint, parent_dir_name);
  string_concat(parent_dir_name, "/..");
  vfs_last_token_in_path(dir_name);
  vfs_umount(vfs_traverse(parent_dir_name, VFS_TRAVERSE_NO_RETURN_NEAREST), dir_name);
  return 0;
}
