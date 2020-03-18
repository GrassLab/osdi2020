#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

int open_port(){
	int fd;

	/*
		O_RDWR: read write
		O_NOCTTY: the process won't control tty terminal
		O_NDELAY: allow RS-232 DCD signal
	*/
	fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	if(fd == -1){
		perror("open_port: Unable to open /dev/ttyUSB0\n");
	}
	else fcntl(fd, F_SETFL, 0);

	return fd;
}

int main()
{
	int tty_fd = open_port();

	char inputStr[128];
	FILE *inputFile_fd = fopen("serial_input.txt", "r");
	int num = fread((void*)inputStr, 128, sizeof(char), inputFile_fd);
	printf("[%d]%s", num, inputStr);

	for(int i=0; i<strlen(inputStr); i++){
		write(tty_fd, inputStr+i, 1);
		if(inputStr[i] == '\n') sleep(1.5);
	}

	return 0;
}