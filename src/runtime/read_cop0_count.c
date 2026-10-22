unsigned int func_80048AA0(void) {
    unsigned int value;
    __asm__ volatile ("mfc0 %0, $9" : "=r" (value));
    return value;
}
