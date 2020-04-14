#include "shell.h"
#include "my_string.h"
#include "uart.h"
#include "mbox.h"

void welcome_msg(){
    uart_puts("-----------------------------------------\n");
    uart_puts("|               welcome!!!              |\n");
    uart_puts("-----------------------------------------\n");
    uart_puts("\n");
    uart_puts("⣿⣿⣟⣽⣿⣿⣿⣿⣟⣵⣿⣿⣿⡿⣳⣫⣾⣿⣿⠟⠻⣿⣿⣿⢻⣿⣿⣿⣿⣷⡽⣿\n");
    uart_puts("⣿⣟⣾⡿⣿⣿⢟⣽⣿⣿⣿⣿⣫⡾⣵⣿⣿⣿⠃⠄⠄⠘⢿⣿⣾⣿⣿⣿⢿⣿⣿⡜\n");
    uart_puts("⡿⣼⡟⣾⣿⢫⣿⣿⣿⣿⡿⣳⣿⣱⣿⣿⣿⡋⠄⠄⠄⠄⠄⠛⠛⠋⠁⠄⠄⣿⢸⣿\n");
    uart_puts("⢳⣟⣼⡿⣳⣿⣿⣿⣿⡿⣹⡿⣃⣿⣿⣿⢳⠁⠄⠄⠄⢀⣀⠄⠄⠄⠄⠄⢀⣿⢿⣿\n");
    uart_puts("⡟⣼⣿⣱⣿⡿⣿⣿⣿⢡⣫⣾⢸⢿⣿⡟⣿⣶⡶⢰⣿⣿⣿⢷⠄⠄⠄⠄⢼⣿⣸⣿\n");
    uart_puts("⣽⣿⢣⣿⡟⣽⣿⣿⠃⣲⣿⣿⣸⣷⡻⡇⣿⣿⢇⣿⣿⣿⣏⣎⣸⣦⣠⡞⣾⢧⣿⣿\n");
    uart_puts("⣿⡏⣿⡿⢰⣿⣿⡏⣼⣿⣿⡏⠙⣿⣿⣤⡿⣿⢸⣿⣿⢟⡞⣰⣿⣿⡟⣹⢯⣿⣿⣿\n");
    uart_puts("⡿⢹⣿⠇⣿⣿⣿⣸⣿⣿⣿⣿⣦⡈⠻⣿⣿⣮⣿⣿⣯⣏⣼⣿⠿⠏⣰⡅⢸⣿⣿⣿\n");
    uart_puts("⡀⣼⣿⢰⣿⣿⣇⣿⣿⡿⠛⠛⠛⠛⠄⣘⣿⣿⣿⣿⣿⣿⣶⣿⠿⠛⢾⡇⢸⣿⣿⣿\n");
    uart_puts("⠄⣿⡟⢸⣿⣿⢻⣿⣿⣷⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⡋⠉⣠⣴⣾⣿⡇⣸⣿⣿⡏\n");
    uart_puts("⠄⣿⡇⢸⣿⣿⢸⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠘⢿⣿⠏⠄⣿⣿⣿⣹\n");
    uart_puts("⠄⢻⡇⢸⣿⣿⠸⣿⣿⣿⣿⣿⣿⠿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣦⣼⠃⠄⢰⣿⣿⢯⣿\n");
    uart_puts("⠄⢸⣿⢸⣿⣿⡄⠙⢿⣿⣿⡿⠁⠄⠄⠄⠄⠉⣿⣿⣿⣿⣿⣿⡏⠄⢀⣾⣿⢯⣿⣿\n");
    uart_puts("⣾⣸⣿⠄⣿⣿⡇⠄⠄⠙⢿⣀⠄⠄⠄⠄⠄⣰⣿⣿⣿⣿⣿⠟⠄⠄⣼⡿⢫⣻⣿⣿\n");
    uart_puts("⣿⣿⣿⠄⢸⣿⣿⠄⠄⠄⠄⠙⠿⣷⣶⣤⣴⣿⠿⠿⠛⠉⠄⠄ ⢸⣿⣿⣿⣿⠃⠄\n");
    uart_puts("\n");
    uart_puts("# ");
}

void run(char *command){
    if (!strcmp(command, "hello")){
        hello();
    }
    else if (!strcmp(command, "help")){
        help();
    }
    else if (!strcmp(command, "timestamp")){
        timestamp();
    }
    else if (!strcmp(command, "reboot")){
        reboot(0);
    }
    else if (!strcmp(command, "hwinfo")){
        get_board_revision();
        get_vc_base_address();
    }
    else if (!strcmp(command, "loadimg")){
        loadimg();
    }
    else if (!strcmp(command, "exc")){
        exc();
    }
    else if (!strcmp(command, "irq")){
        irq();
    }
    else {
        uart_puts("Error: command not found, try <help>.\n");
    }
}

void help(){
    uart_puts("<hello>: print Hello World!\n");
    uart_puts("---\n");
    uart_puts("<timestamp>: print current timestamp.\n");
    uart_puts("---\n");
    uart_puts("<reboot>: reboot rpi3.\n");
    uart_puts("---\n");;
    uart_puts("<hwinfo>: get hardware information.\n");
    uart_puts("---\n");
    uart_puts("<loadimg>: load kernel image and run.\n");
    uart_puts("---\n");
    uart_puts("<exc>: take a exception by \"svc 1\".\n");
    uart_puts("---\n");
    uart_puts("<irq>: enable timer interrupt.\n");
    uart_puts("---\n");
    uart_puts("<help>: print all available commands.\n");
}

void hello(){
    uart_puts("Hello World!\n");
}

void timestamp(){
    unsigned long long counter, freq;
    char res[30];
    char f_res[10];
    asm volatile("mrs %0, cntpct_el0" : "=r"(counter));
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));

    ftoa((float)counter/freq, res, f_res);
    uart_send('[');
    uart_puts(res);
    uart_send('.');
    uart_puts(f_res);
    uart_puts("]\n");
}

void reboot(int tick){
    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | tick;
    while(1);
}

void get_board_revision(){
	mbox[0] = 7*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = GET_BOARD_REVISION;   // get board revision
    mbox[3] = 4;                    // buffer size
    mbox[4] = 0;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox_call(8);

    char res[30];
    unsign_itohexa(mbox[5], res);
    uart_puts("board revision: 0x");
	uart_puts(res);
	uart_puts("\n");
}

void get_vc_base_address(){
	mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = GET_VC_BASE_ADDRESS;   // get board revision
    mbox[3] = 8;                    // buffer size
    mbox[4] = 0;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox_call(8);

    char res[30];
    unsign_itohexa(mbox[5], res);
    uart_puts("VC Core base address: 0x");
	uart_puts(res);
	uart_puts("\n");
}

void loadimg(){
    char buf[10];
    int i=0;

    uart_puts("Please input image size: ");
    buf[i] = uart_getc();
    while(buf[i] != '\n' && buf[i] != '\r'){
        uart_send(buf[i++]);
        buf[i] = uart_getc();
    }
    buf[i] = '\0';
    int size = atoi(buf);
    uart_puts("\n");

    i = 0;
    uart_puts("Please input image load address: ");
    buf[i] = uart_getc();
    while(buf[i] != '\n' && buf[i] != '\r'){
        uart_send(buf[i++]);
        buf[i] = uart_getc();
    }
    buf[i] = '\0';
    uart_puts("\n");
    char *load_addr = (char *)hexatoi(buf);

    uart_puts("Please send image...\n");

    char *base = load_addr;


    while(size--){
        *load_addr = uart_getbyte();
        load_addr++;
    }

    ((void(*)(void))base)();
}

void exc(){
    asm volatile ("svc 1");
}

void irq(){
    asm(
        "mov x0, 1;"
        "msr cntp_ctl_el0, x0;"
        "mrs x0, cntfrq_el0;"
        "msr cntp_tval_el0, x0;"
        //"mov x0, 2;"
        //"ldr x1, =0x40000040;"
        //"str x0, [x1];"
    );
    *CORE0_TIMER_IRQ_CTRL = 2;
}
