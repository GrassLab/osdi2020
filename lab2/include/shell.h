#include "config.h"

char buff[50];

void shell();
void _unsign_arr_to_digit(unsigned num, char* buf, unsigned len);
int recv_img_size();
void copy_self_kernel(char *new_address);
void copy_and_jump_to_kernel(char *new_address);

