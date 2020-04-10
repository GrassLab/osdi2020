void disable_irq();
void enable_irq();
void set_HCR_EL2_IMO();
void core_timer_counter();

void local_timer_init();
void local_timer_handler();
void local_timer_counter();

// define in start.S
void core_timer_enable();
void core_timer_handler();


//void timer_irq_setup();
//void handle_timer_irq();
