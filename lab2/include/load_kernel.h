#ifndef LOAD_KERNEL_H
#define LOAD_KERNEL_H

void load_kernel_at_target_address(const char * target_address);
int read_kernel_size();
unsigned long long int read_kernel_address();

#endif
