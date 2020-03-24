#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include "simshell.h"

void send_int(simshell *sim, int number) {
    char send_str[4];
    send_str[0] = (char)((number >> 24) & 0xFF);
    send_str[1] = (char)((number >> 16) & 0xFF);
    send_str[2] = (char)((number >> 8) & 0xFF);
    send_str[3] = (char)(number & 0xFF);
    write(sim->utf, send_str, 4);
    return;
}

int read_line(simshell *sim, char *buffer)
{
    char input = 'c';
    int  i = 0;

    memset(buffer, '\0', 100);
    while(input != '\n' && input != '\0') {
        if(read(sim->utf, &input, 1) > 0) {
            buffer[i] = input;
            i = i + 1;
        }
    }
    buffer[i] = '\0';
    return i;
}

int read_int(simshell *sim) {
    char buff[100];
    int num = 0;
    int index;
    index = read_line(sim, buff);
    return atoi(buff);
}

bool send_kernel(simshell *sim)
{
    if (sim->utf == -1) {
        printf("Fail to read user input\n");
	    return false;
    }
    FILE *fp;
    fpos_t pos;
    char send_size[100];
    unsigned char buff;
    int file_size;
    int return_size;
    int check_sum = 0;
    int return_checksum;

    fp = fopen("../kernel8.img", "rb");
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    sprintf(send_size, "%d\n", file_size);
    printf("send file size %s", send_size);

    write(sim->utf, send_size, strlen(send_size));
    return_size = read_int(sim);
    printf("get %d bytes\n", return_size);
    if (return_size == file_size) {
        printf("confirm file size\n");
    }
    else {
        printf("wrong size\n");
    }

    if (!fp) {
        printf("Can't open the file\n");
        return 1;
    }
    while( fread(&buff, sizeof(char), 1, fp) == 1 ) {
        check_sum += buff;
        write(sim->utf, &buff, 1);
    }
    printf("Validating checksum %d...\n", check_sum);
    return_checksum = read_int(sim);
    printf("return_checksum %d\n", return_checksum);
    if (check_sum == return_checksum) {
        printf("kernel rewrited\n");
    }
    else {
        printf("fail rewrite\n");
    }
    return 1;
}

bool set_up_options(simshell *sim)
{
    sim->utf = -1;
    sim->utf = open("/dev/pts/2", O_RDWR | O_NOCTTY );
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
    else if (sim->rx_buffer[rx_length - 1] == '\n') {
        sim->rx_buffer[rx_length - 1] = '\0';
        return 1;
    }
    else {
        return 0;
    }
    return 0;
}