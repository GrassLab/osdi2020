#include "mini_uart.h"
#include "sched.h"
#include "syscall.h"

uint32_t do_get_taskid(void) {
  return get_current_task()->id;
}

size_t do_uart_read(void *buf, size_t count) {
  mutex_lock(&uart_lock);
  mini_uart_getn(/* verbose */ false, buf, count);
  mutex_unlock(&uart_lock);
  /* For now, just assume all "count" bytes can be read. */
  return count;
}

size_t do_uart_write(const void *buf, size_t count) {
  mutex_lock(&uart_lock);
  mini_uart_putn(buf, count);
  mutex_unlock(&uart_lock);
  /* For now, just assume all "count" bytes can be written. */
  return count;
}

void *syscall_table[] = {
  do_get_taskid,
  do_uart_read,
  do_uart_write
};
