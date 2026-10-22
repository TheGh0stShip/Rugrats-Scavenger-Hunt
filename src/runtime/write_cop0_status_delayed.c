void func_80048D60(unsigned int value) {
    __asm__ volatile ("mtc0 %0, $12\n\tnop" :: "r" (value));
}
