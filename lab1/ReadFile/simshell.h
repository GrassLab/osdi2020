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

int read_pi_return(simshell *sim);

#endif 
