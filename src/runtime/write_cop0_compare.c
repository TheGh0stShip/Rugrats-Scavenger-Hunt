void func_800495C0(unsigned int value) {
    __asm__ volatile ("mtc0 %0, $11" :: "r" (value));
}
