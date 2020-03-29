double get_time(){
    register double frequency, counter;
    double time;
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(frequency));
    asm volatile ("mrs %0, cntpct_el0" : "=r"(counter));
    time = counter / frequency;
    return time;
}

int atoi(char *str){
    int res = 0; 
    for (int i = 2; str[i] != '\0'; ++i) 
        res = res * 16 + str[i] - '0';  
    return res; 
}