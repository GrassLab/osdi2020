void disable_irq();
void enable_irq();
void set_HCR_EL2_IMO();
void uart_irq_enable();

void interrupt_handler();

//  local_timer
void local_timer_init();
void local_timer_counter();
void local_timer_handler();

// core_timer 
void core_timer_enable();
void core_timer_enable_user();
void core_timer_counter();
void core_timer_handler(); // define in start.S


//void timer_irq_setup();
