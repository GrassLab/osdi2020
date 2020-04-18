unsigned int get_kernel_size();
void copy_loader(char *copy_addr, unsigned int size);
void recieve_kernel(char *load_addr, unsigned int size, unsigned long copy_addr);
void load_kernel_img();