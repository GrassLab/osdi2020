void foo(){
    while(1) {
        // printf("Task id: %d\n", current -> taskid);
        delay(1000000);
        schedule();
    }
}

void idle(){
    while(1){
        schedule();
        delay(1000000);
    }
}

void el1_main(){
    // set up serial console
    uart_init();

    core_timer_enable();

    int N = 3;
    for(int i = 0; i < N; ++i) { // N should > 2
        privilege_task_create(foo, i);
    }

    idle();
    
}