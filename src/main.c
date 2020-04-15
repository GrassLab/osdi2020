#include "shell.h"

#define CMD_LEN 128

int main() {
    shell_init();

    while (1) {
        char cmd[CMD_LEN];
        shell_input(cmd);
        shell_controller(cmd);
    }
}