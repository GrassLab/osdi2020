#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include "simshell.h"

bool set_up_options(simshell *sim)
{
    sim->utf = -1;
    sim->utf = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

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
    if (count < 0) {
        printf("UART TX error\n");
	return false;
    }
    return true;
}
