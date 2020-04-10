void disable_irq();
void enable_irq();
void set_HCR_EL2_IMO();

//  local_timer
void local_timer_init();
void local_timer_counter();
void local_timer_handler();

// core_timer 
void core_timer_enable();
void core_timer_counter();
void core_timer_handler(); // define in start.S


//void timer_irq_setup();
//void handle_timer_irq();
