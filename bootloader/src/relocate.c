extern unsigned char _begin, _end, __boot_loader;

__attribute__((section(".text.relocate"))) void relocate() {
    unsigned long kernel_size = (&_end - &_begin);
    unsigned char *new_bl = (unsigned char *)&__boot_loader;
    unsigned char *bl = (unsigned char *)&_begin;

    while (kernel_size--) {
        *new_bl++ = *bl;
        *bl++ = 0;
    }

    void (*start)(void) = (void *)&__boot_loader;
    start();
}