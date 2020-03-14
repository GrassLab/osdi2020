#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include "simshell.h"

bool set_up_options(simshell *sim)
{
    sim->utf = -1;
    sim->utf = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY );
    if(sim->utf == -1) {
        printf("Fail to set_up_options\n");
        return false;
    }

    tcgetattr(sim->utf, &sim->options);
    sim->options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    sim->options.c_iflag = IGNPAR;
    sim->options.c_oflag = 0;
    sim->options.c_lflag = 0;
    tcflush(sim->utf, TCIFLUSH);
    tcsetattr(sim->utf, TCSANOW, &sim->options);
    return true;
}

bool read_write_user_input(simshell *sim)
{
    if (sim->utf == -1) {
        printf("Fail to read user input\n");
	    return false;
    }
    scanf("%s", sim->tx_buffer);
    int count = write(sim->utf, sim->tx_buffer, strlen(sim->tx_buffer));
    write(sim->utf, "\n", 1);
    if (count < 0) {
        printf("UART TX error\n");
	    return false;
    }
    return true;
}

int read_pi_return(simshell *sim)
{
    if (sim->utf == -1) {
        printf("Fail to read pi output\n");
        return false;
    }
    memset(sim->rx_buffer,'\0' ,51);
    int rx_length = read(sim->utf, (void*)sim->rx_buffer, 50);
    if (rx_length < 0) {
        printf("Fail to read pi output\n");
        return -1;
    }
    else if (rx_length == 0) {
        return -1;
    }
    else if (sim->rx_buffer[rx_length-1] == '\r') {
        sim->rx_buffer[rx_length-1] = '\n';
        return 0;
    } 
    else if (sim->rx_buffer[rx_length-1] != '\n') {
        return 0;
    }
    else if (sim->rx_buffer[rx_length-1] == '\n'){
        sim->rx_buffer[rx_length] = '\0';
        return 1;
    }
    return 0;
}