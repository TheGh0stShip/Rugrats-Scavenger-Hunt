unsigned int func_80004CA4(void) {
    unsigned int value;
    __asm__ volatile ("mfc0 %0, $12" : "=r" (value));
    return value;
}
