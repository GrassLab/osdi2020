__attribute__((section(".text.relocate"))) void relocate() {
    extern unsigned char _begin;
    void (*start)(void) = (void*) &_begin;
    start();
}