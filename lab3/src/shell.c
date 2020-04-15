#include "io.h"
#include "util.h"
#include "time.h"
#include "power.h"
#include "string.h"
#include "mbox.h"
#include "info.h"
#include "map.h"
#include "timer.h"
#include "irq.h"

#ifndef WITHOUT_LOADER
#include "loadimg.h"
#endif

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

char *exec_ptr = 0;

char *shell_read_line(char *ptr){
    print("# ");
    char *beg = ptr--;
    do {
        *(++ptr) = getchar();
        switch(*ptr){
            case 4:
                ptr = buffer + 1;
                *ptr = '\r';
                *buffer = 4;
                break;
            case 8:
            case 127:
                ptr--;
                if(ptr >= buffer){
                    ptr--;
                    print("\b \b");
                }
                break;
            case 12:
                *ptr = 0;
                ptr--;
                print("\e[1;1H\e[2J");
                print("# ", beg);
                break;
            case 21:
                ptr--;
                while(ptr >= buffer){
                    if(*ptr == '\t')
                        print("\b\b\b\b\b\b");
                    else
                        print("\b \b");
                    ptr--;
                }
                break;
            default:
                putchar(*ptr);
        }

    } while(ptr < buffer || (!strchr("\r", *ptr)));
    while(ptr >= buffer && strchr(" \r\t", *ptr)) ptr--;
    *(++ptr) = 0; puts("");
    while(beg < ptr && strchr(" \r\t\n", *beg)) beg++;
    return beg;
}

#define EQS(xs, ys) (!strcmp(xs, ys))
int shell_execute(char *cmd){
    if(EQS("hello", cmd)){
        println("Hello World!");
    }
    else if(EQS("help", cmd)){
        println("hello : print Hello World!");
        println("clear : clean the screen");
        println("timestamp : show timestamp");
        println("reboot : reboot the device");
        println("help : print all available commands");
#ifdef BUILD_STAMP
#define xstr(a) str(a)
#define str(a) #a
        println("BUILD @ ", xstr(BUILD_STAMP));
#endif
    }
    else if(EQS("timestamp", cmd)){
        timestamp();
    }
    else if(EQS("sleep", cmd)){
        sleep(6);
    }
    else if(EQS("reboot", cmd)){
        puts("rebooting...");
        reboot();
        return -1;
    }
    else if(EQS("exit", cmd) || cmd[0] == 4){
        //*DISABLE_IRQS_1 = (SYSTEM_TIMER_IRQ_1 | AUX_IRQ_MSK);
        //__asm__ volatile ("msr daifset, #0xf":::"memory");
        __asm__ volatile("stp x8, x9, [sp, #-16]!");
        __asm__ volatile("mov x8, #2");
        __asm__ volatile("svc #0");
        __asm__ volatile("ldp x8, x9, [sp], #16");
        return -1;
    }
    else if(EQS("clear", cmd)){
        print("\e[1;1H\e[2J");
    }
#ifndef WITHOUT_LOADER
    else if(EQS("loadimg", cmd)){
        loadimg();
    }
#endif
    else if(EQS("exc", cmd)){

#if defined(RUN_ON_EL1) || defined(RUN_ON_EL2)
        get_current_el();
#endif

#if 1
        int *sp; 
        __asm__ volatile ("mov %0, sp" : "=r"(sp));
        printf("sp = 0x%x\n", sp);
#endif

        unsigned long gpr_ctx[31]; 
        unsigned long ret_ctx[31]; 
#define SAVE_GPRCTX(x, ctxp)  __asm__ volatile ("str x" #x ", %0" : "=m"(ctxp[x]));

#define save_ctx 1
#if save_ctx
#define SAVE_PRECTX(x) SAVE_GPRCTX(x, gpr_ctx) 
        MAP(SAVE_PRECTX, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30);
#endif

        __asm__ volatile ("svc #1");

#define SAVE_RETCTX(x) SAVE_GPRCTX(x, ret_ctx) 
        MAP(SAVE_RETCTX, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30);

#ifdef diffctx
        for(int i = 0; i < 31; i++)
            printf("ctx diff (pre:ret) x%d (%d:%d)\n", i, gpr_ctx[i], ret_ctx[i]);
#endif
    }
    else if(EQS("brk", cmd)){
#if 1
        int *sp;
        __asm__ volatile ("mov %0, sp" : "=r"(sp));
        printf("sp = 0x%x\n", sp);
#endif
        __asm__ volatile ("brk #1");
        puts("ret from brk");
    }
    else if(EQS("irq", cmd)){
        //sys_timer_init();
        local_timer_init();
        //core_timer_init();
    }
    else if(EQS("board", cmd)){
        if(get_board_revision())
            printf("0x%x" NEWLINE, mbox[5]);
        else
            puts("get_board_reversion() failed");
    }
    else if(EQS("vcaddr", cmd)){
        if(get_vc_memaddr())
            printf("0x%x\n", mbox[5]); // it should be 0xa020d3 for rpi3 b+
        else
            puts("get_vc_memaddr() failed");
    }
#ifdef TEST
    else if(EQS("bss", cmd)){
        char *beg, *end;
        __asm__ volatile ("adr %0, __bss_beg" : "=r"(beg));
        __asm__ volatile ("adr %0, __bss_end" : "=r"(end));
        for(char *p = beg; p != end; p++){
            print("0x");
            _print_ULL_as_number((unsigned long long)p, 16);
            println(": ", (int)*p);
        }
    }
#endif
    else if(strlen(cmd)){
        print("command not found: ", cmd, NEWLINE);
        return 1;
    }
    return 0;
}

void shell_stuff_line(char c){
    static char *ptr = buffer - 1;

    *(++ptr) = c;
    switch(*ptr){
        case 4:
            ptr = buffer + 1;
            *ptr = '\r';
            *buffer = 4;
            break;
        case 8:
        case 127:
            ptr--;
            if(ptr >= buffer){
                ptr--;
                print("\b \b");
            }
            break;
        case 12:
            *ptr = 0;
            ptr--;
            print("\e[1;1H\e[2J");
            print("# ", buffer);
            break;
        case 21:
            ptr--;
            while(ptr >= buffer){
                if(*ptr == '\t')
                    print("\b\b\b\b\b\b");
                else
                    print("\b \b");
                ptr--;
            }
            break;
        default:
            putchar(*ptr);
    }

    if(ptr >= buffer && strchr("\r\n", *ptr)){
        exec_ptr = buffer;
        while(ptr >= buffer && strchr(" \r\t", *ptr)) ptr--;
        *(++ptr) = 0; puts("");
        while(exec_ptr < ptr && strchr(" \r\t\n", *exec_ptr)) exec_ptr++;
        ptr = buffer - 1;
        puts("wait to exec");
        //shell_execute(beg);
        //exec_shell = 1;
        //print("# ");
        //__asm__ volatile("stp x8, x9, [sp, #-16]!");
        //__asm__ volatile("mov x8, #4");
        //__asm__ volatile("svc #0");
        //__asm__ volatile("ldp x8, x9, [sp], #16");
    }
    else if(ptr >= buffer + BUFFER_SIZE){
        puts("buffer size isn't enough... cleared.");
        ptr = buffer - 1;
    } 
}


int shell_loop(){
    while(shell_execute(shell_read_line(buffer)) >= 0);
    return 0;
}
