void func_80004CB0(unsigned int value) {
    __asm__ volatile ("mtc0 %0, $12" :: "r" (value));
}
