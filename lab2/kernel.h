unsigned int get_kernel_size();
unsigned long calc_loader_copy_addr(unsigned long load_addr_ul);
void copy_loader_and_jump(char *copy_addr, char *load_kernel_addr, unsigned int size);
void recieve_kernel(char *load_addr, unsigned int size);