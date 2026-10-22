void func_80048920(unsigned int mask) {
    __asm__ volatile (
        "mfc0 $8, $12\n\t"
        "or $8, $8, %0\n\t"
        "mtc0 $8, $12\n\t"
        "nop\n\t"
        "nop"
        :: "r" (mask)
        : "$8"
    );
}
