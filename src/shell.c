#include "io.h"
#include "util.h"
#include "time.h"
#include "power.h"
#include "string.h"

char buffer[1024];

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
    }
    else if(EQS("exit", cmd) || cmd[0] == 4){
        return -1;
    }
    else if(EQS("clear", cmd)){
        print("\e[1;1H\e[2J");
    }
    else if(strlen(cmd)){
        print("command not found: ", cmd, NEWLINE);
        return 1;
    }
    return 0;
}

int shell_loop(){
    //while(1) shell_read_line(buffer);
    while(shell_execute(shell_read_line(buffer)) >= 0);
    return 0;
}
