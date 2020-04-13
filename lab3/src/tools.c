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
void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        int tmp;
        tmp = *(str + start);
        *(str + start) = *(str + end);
        *(str + end) = tmp;

        start++;
        end--;
    }
}
// Implementation of itoa()
char *itoa(int num, char *str, int base)
{
    int i = 0;
    int isNegative = 0;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}
int atoi(const char *str)
{
    int res = 0; // Initialize result

    // Iterate through all characters of input string and
    // update result
    for (int i = 0; str[i] != '\0' && str[i] != '\n'; ++i)
        res = res * 10 + str[i] - '0';

    // return result.
    return res;
}


int cmd_box(char * command){
    if(my_strcmp(command, "help")==1)return 1;
    if(my_strcmp(command, "hello")==1)return 2;
    if(my_strcmp(command, "reset")==1)return 3;
    if(my_strcmp(command, "timestamp")==1)return 4;
    if(my_strcmp(command, "loadimage")==1)return 5;
    if(my_strcmp(command, "exc")==1)return 6;
    return -1;
}
void process_cmd(char * command){
    switch (cmd_box(command)){
        case 1:
            uart_puts("help        :show every commands.\n");
            uart_puts("hello       :say hello to you.\n");
            uart_puts("reset       :reboot the mechine.\n");
            uart_puts("loadimage   :load kernel image.\n");
            uart_puts("exc         :create exception and print information.\n");
            break;
        case 2:
            uart_puts("Hello World!");
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
            uart_puts("Load Image!\n");
            load_image();
            break;        
        case 6:
            asm volatile ("svc #1");
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
#define INPUT_BUFFER_SIZE 256
char cmd[INPUT_BUFFER_SIZE];
void get_number(){
    uart_gets(cmd, INPUT_BUFFER_SIZE);
    int image_size = atoi(cmd);
    uart_send_int(image_size);
}
void load_image(){
    
    uart_gets(cmd, INPUT_BUFFER_SIZE);
    int image_size = atoi(cmd);
    uart_puts("Kernel size is: ");
    uart_send_int(image_size);
    uart_send('\n');

    uart_puts("Input Kernel load address: ");
    uart_gets(cmd, INPUT_BUFFER_SIZE);
    int address = atoi(cmd);
    uart_puts("Kernel load address is: ");
    uart_hex(address);
    uart_send('\n');

    copy_and_jump_to_kernel((void *)(long)address, image_size);
}
void copy_and_jump_to_kernel(char *new_address, int kernel_size) {
    //char *kernel = _start;
    char *kernel = __kernel_start;
    char *end = __bss_end;
    char *copy = (char *)(TMP_KERNEL_ADDR);
    uart_puts("begin of copy kernel\n");
    while (kernel <= end) {
        *copy = *kernel;
        kernel++;
        copy++;
    }
    uart_puts("end of copy kernel\n");

    char *kernell = new_address;
    uart_puts("Find New func address! \n");
    uart_puts("Kernel_size: ");
    uart_send_int(kernel_size);

    for (int i = 0; i < kernel_size; i++) {
        unsigned char c = uart_getc();
        kernell[i] = c;
    }
    uart_puts("done\n");
    uart_puts("Jump to new kernel\n");
    //branch_to_address((unsigned long int *)new_address);
    asm volatile("br %0" : "=r"((unsigned long int*)new_address));
}
