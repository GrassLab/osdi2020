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
    while(input != '\n') {
        if(read(sim->utf, &input, 1) > 0) {
            buffer[i] = input;
            i = i + 1;
        }
    }
    buffer[i - 1] = '\0';
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
    char address_array[10];
    char rx_buffer[20];
    unsigned char buff;
    int file_size;
    int return_size;
    int check_sum = 0;
    int return_checksum;
    unsigned long int input_address;
    printf("input kernel address :");
    scanf("%lx", &input_address);
    sprintf(address_array, "%lu\n", input_address);
    write(sim->utf, address_array, strlen(address_array));
    read_line(sim, rx_buffer);
    printf("%s\n", rx_buffer);
    read_line(sim, rx_buffer);
    printf("%s\n", rx_buffer);

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
    printf("validating checksum %d...\n", check_sum);
    return_checksum = read_int(sim);
    printf("return checksum %d\n", return_checksum);
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

bool read_vc_address(simshell *sim) 
{
    char rx_buffer[100];
    /*
    uart_send_string("base address in bytes\n");
    uart_send_hex(mbox[5]);
    uart_send_string("size in bytes\n");
    uart_send_hex(mbox[6]);
    */
    read_line(sim, rx_buffer);
    printf("%s\n", rx_buffer);
    read_line(sim, rx_buffer);
    printf("%s\n", rx_buffer);
    return 1;
}

bool read_revision(simshell *sim)
{
    char rx_buffer[100];
    read_line(sim, rx_buffer);
    printf("%s\n", rx_buffer);
    return 1;
}