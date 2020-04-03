#ifndef SIMSHELL_H
#define SIMSHELL_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>

typedef struct SIMSHELL {
    int utf;
    struct termios options;
    char tx_buffer[51];
    char rx_buffer[51];
}simshell;

bool set_up_options(simshell *sim);

bool read_write_user_input(simshell *sim);

bool send_kernel(simshell *sim);

bool read_vc_address(simshell *sim);

int read_line(simshell *sim, char *buffer);

bool read_revision(simshell *sim);
#endif 