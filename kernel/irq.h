#ifndef IRQ
#define IRQ

#define CORE0_INTR_SRC (unsigned int* )0x40000060
#define IRQ_BASIC_PENDING (unsigned int *)(MMIO_BASE + 0xb200)
#define ENABLE_IRQ2 (unsigned int *)(MMIO_BASE + 0xb214)

extern void init_irq ();
extern void enable_irq ();
extern void disable_irq ();
void irq_router ();
void kernel_irq_router ();
int is_core_timer ();
int is_local_timer ();
void init_uart_irq ();

#endif /* ifndef IRQ */
