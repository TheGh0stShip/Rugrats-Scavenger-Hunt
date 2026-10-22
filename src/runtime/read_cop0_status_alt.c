unsigned int func_80048CC0(void) {
    unsigned int value;
    __asm__ volatile ("mfc0 %0, $12" : "=r" (value));
    return value;
}
