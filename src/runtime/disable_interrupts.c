unsigned int func_80048900(void) {
    register unsigned int result __asm__("$2");

    __asm__ volatile (
        "mfc0 $8, $12\n\t"
        "addiu $1, $0, -2\n\t"
        "and $9, $8, $1\n\t"
        "mtc0 $9, $12\n\t"
        "andi %0, $8, 1\n\t"
        "nop"
        : "=r" (result)
        :
        : "$1", "$8", "$9"
    );
    return result;
}
