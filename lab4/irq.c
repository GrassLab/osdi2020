#include <stdint.h>
#include "meta_macro.h"
#include "string_util.h"
#include "irq.h"
#include "uart.h"
#include "timer.h"
#include "schedule.h"
#include "signal.h"
#include "task.h"
#include "sys.h"

void irq_int_enable(void)
{
  // Clear interrupt mask for d, a, (i), f
  asm volatile("msr daifclr, #0x2");
}

void irq_int_disable(void)
{
  // Set interrupt mask for d, a, (i), f
  asm volatile("msr daifset, #0x2");
}

void irq_el1_handler(void)
{
  char string_buff[0x100];
  static int local_timer_count = 0;
  if(CHECK_BIT(*CORE0_IRQ_SRC, 1))
  {
    /* ARM core timer interrupt, update*/
    timer_set_core_timer_approx_ms(SCHEDULE_TIMEOUT_MS);

    schedule_update_quantum_count();

    /* check SIGKILL */
    if(CHECK_BIT(task_get_current_task_signal(), SIGKILL))
    {
      sys_exit(1);
    }

    /* check reschedule bit */
    if(schedule_check_self_reschedule())
    {
      schedule_yield();
    }
  }
  else if(CHECK_BIT(*LOCAL_TIMER_CONTROL_REG, 31))
  {
    ++local_timer_count;
    uart_puts("ARM local time interrupt \"");
    string_longlong_to_char(string_buff, local_timer_count);
    uart_puts(string_buff);
    uart_puts("\" received\n");
    timer_clear_local_timer_int_and_reload();
    if(local_timer_count == DISABLE_TIMER_COUNT)
    {
      timer_disable_local_timer();
      local_timer_count = 0;
    }
  }
  else if(CHECK_BIT(*INT_BASIC_PENDING, 19))
  {
    static int tx_bug_fix = 0;
    // UART interrupt
    // [19] is GPU IRQ 59 which is uart_int (59)
    /* TX int: availible to write*/
    if(CHECK_BIT(*UART_MIS, 5) & CHECK_BIT(*UART_RIS, 5))
    {
      if(!tx_bug_fix)
      {
        tx_bug_fix = 1;
        /* clear intended interrupt */
        *UART_ICR = 0x5;
        return;
      }

      if(!QUEUE_EMPTY(uart_tx_queue))
      {
        while(!CHECK_BIT(*UART_FR, 5)) /* while fifo is not full */
        {
          /* dump data */
          if(!QUEUE_EMPTY(uart_tx_queue))
          {
            *UART_DR = (uint32_t)QUEUE_POP(uart_tx_queue);
          }
          else
          {
            break;
          }
        }
      }
      /* Nothing to do so clear tx interrupt */
      *UART_ICR = 0x20;
      return;
    }
    /* RX int */
    else if(CHECK_BIT(*UART_MIS, 4) & CHECK_BIT(*UART_RIS, 4))
    {
      while(!CHECK_BIT(*UART_FR, 4)) /* rxfe not set -> fifo is not empty -> move data into queue */
      {
        if(!QUEUE_FULL(uart_rx_queue))
        {
          QUEUE_PUSH(uart_rx_queue, (char)*UART_DR);
        }
        else
        {
          uart_puts("RX QUEUE EXPLODE\n");
          while(1);
          /* There's nothing we can do for now, the program will hang. Try enlarge the queue */
        }
      }
      /* Cleart interrupt for safety */
      *UART_ICR = 0x10;
      return;
    }
  }
  else
  {
    uart_puts("Unidentified interrupt source\nEntering busy loop");
    while(1);
  }
  return;
}

