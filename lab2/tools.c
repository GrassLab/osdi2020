#include "tools.h"

int my_strcmp(char * str1, char * str2){
    int i=0;    
    while(str1[i]!='\0'){        
        if(str1[i] != str2[i])return 0;        
        i++;
    }
    if(str2[i]!='\0')
        return 0;
    return 1;
}
int cmd_box(char * command){
    if(my_strcmp(command, "help")==1)return 1;
    if(my_strcmp(command, "hello")==1)return 2;
    if(my_strcmp(command, "reset")==1)return 3;
    if(my_strcmp(command, "timestamp")==1)return 4;
    if(my_strcmp(command, "loadimage")==1)return 5;
    return -1;
}
void process_cmd(char * command){
    switch (cmd_box(command)){
        case 1:
            uart_puts("help        :show every commands.\n");
            uart_puts("hello       :say hello to you.\n");
            uart_puts("reset       :reboot the mechine.\n");
            uart_puts("loadimage   :load kernel image.\n");
            break;
        case 2:
            uart_puts("Hello World33!");
            break;
        case 3:
            uart_puts("Rebooting");
            my_reset();
            break;
        case 4:
            uart_puts("Get Time!");
            get_timestamp();
            break;
        case 5:
            uart_puts("Load Image!");
            load_image();
            break;
        default:
            uart_puts(command);
            uart_puts(": command not found");
            break;
    }
    command[0] = '\0';
}


void print_hello(){
    // say hello
    uart_puts("  _____ ____    _    ___ _        ___  ____   \n");
    uart_puts(" |_   _/ ___|  / \\  |_ _( )___   / _ \\/ ___|  \n");
    uart_puts("   | | \\___ \\ / _ \\  | ||// __| | | | \\___ \\  \n");
    uart_puts("   | |  ___) / ___ \\ | |  \\__ \\ | |_| |___) | \n");
    uart_puts("   |_| |____/_/   \\_\\___| |___/  \\___/|____/  \n");
                                                              
    uart_puts("     ___           ___           ___       ___       ___        \n");
    uart_puts("    /\\__\\         /\\  \\         /\\__\\     /\\__\\     /\\  \\       \n");
    uart_puts("   /:/  /        /::\\  \\       /:/  /    /:/  /    /::\\  \\      \n");
    uart_puts("  /:/__/        /:/\\:\\  \\     /:/  /    /:/  /    /:/\\:\\  \\     \n");
    uart_puts(" /::\\  \\ ___   /::\\~\\:\\  \\   /:/  /    /:/  /    /:/  \\:\\  \\    \n");
    uart_puts("/:/\\:\\  /\\__\\ /:/\\:\\ \\:\\__\\ /:/__/    /:/__/    /:/__/ \\:\\__\\   \n");
    uart_puts("\\/__\\:\\/:/  / \\:\\~\\:\\ \\/__/ \\:\\  \\    \\:\\  \\    \\:\\  \\ /:/  /   \n");
    uart_puts("     \\::/  /   \\:\\ \\:\\__\\    \\:\\  \\    \\:\\  \\    \\:\\  /:/  /    \n");
    uart_puts("     /:/  /     \\:\\ \\/__/     \\:\\  \\    \\:\\  \\    \\:\\/:/  /     \n");
    uart_puts("    /:/  /       \\:\\__\\        \\:\\__\\    \\:\\__\\    \\::/  /      \n");
    uart_puts("    \\/__/         \\/__/         \\/__/     \\/__/     \\/__/       \n");

}
char s[500]="";
char * int2char(int input){
    
    unsigned long bit=1, tmp=input, in=input;
    int ind=0;
    while(tmp/10>0){
        bit*=10;
        tmp/=10;
    }
    while(in){
        s[ind] = (in/bit)+'0';
        in%=bit;
        bit/=10;
        ind++;
    }
    s[ind] = '\0';
    return s;
}
int get_cpu_time(){
    int t=0;
    asm volatile("mrs %0, cntpct_el0" : "=r"(t)); // get cpu count
    return t;
}
int get_cpu_clock(){
    int t=0;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(t)); // get cpu clock frequence
    return t;
}
void get_timestamp(){       
    //unsigned long cpu_time = get_cpu_time();
    //unsigned long cpu_clock = get_cpu_clock();
    int cpu_time, cpu_clock;
    asm volatile("mrs %0, cntpct_el0" : "=r"(cpu_time)); 
    asm volatile("mrs %0, cntfrq_el0" : "=r"(cpu_clock));
    uart_puts("\ncpu_time:\n");
    uart_puts(int2char(cpu_time));
    uart_puts("\ncpu_clock:\n");
    uart_puts(int2char(cpu_clock));
    int time_int = cpu_time/cpu_clock;
    uart_puts("\ntime_int:\n");
    uart_puts(int2char(time_int));
    int time_dec = (cpu_time*1000/cpu_clock) - time_int*1000;
    uart_puts("\ntime_dec:\n");
    uart_puts(int2char(time_dec));
    char * time_int_char = int2char(time_int);
    char * time_dec_char = int2char(time_dec);
    uart_puts("\ntime:");
    uart_puts("\n");
    uart_puts(time_int_char);
    uart_puts(".");
    uart_puts(time_dec_char);
}

extern char __bss_end[];
extern char __kernel_start[];
extern void branch_to_address( void * );

#define  TMP_KERNEL_ADDR  0x00100000

void load_image(){
    // uart_puts("in function:Load Image!\n");
    copy_current_kernel_and_jump((char *)TMP_KERNEL_ADDR);
}
void copy_and_jump_to_kernel() {
    int kernel_size = uart_read_int();
    // Confirm kernel size
    // uart_puts(int2char(kernel_size));

    char *kernel = (char *)0x80000;

    int checksum = 0;

    for (int i = 0; i < kernel_size; i++) {
        char c = uart_getc();   
        uart_puts(int2char(i));     
        uart_puts("\n");
        checksum += c;
        kernel[i] = c;
    }

    // uart_send_int(checksum);

    uart_puts("Done copying kernel\n"); 

    branch_to_address((void *)0x80000);
}

void copy_current_kernel_and_jump(char *new_address) {
    char *kernel = (char *)0x80000;
    char *end = __bss_end;

    char *copy = new_address;

    while (kernel <= end) {
        *copy = *kernel;
        kernel++;
        copy++;
    }

    // Cast the function pointer to char* to deal with bytes.
    char *original_function_address = (char *)&copy_and_jump_to_kernel;

    // Add the new address (we're assuming that the original kernel resides in
    // address 0). copied_function_address should now contain the address of the
    // original function but in the new location.
    char *copied_function_address =
        original_function_address + (long)new_address;

    // Cast the address back to a function and call it.
    void (*call_function)() = (void (*)())copied_function_address;
    call_function();
}