unsigned int func_80048D50(unsigned int value) {
    unsigned int previous;

    __asm__ volatile(
        "cfc1 %0, $31\n"
        "ctc1 %1, $31"
        : "=r"(previous)
        : "r"(value)
    );
    return previous;
}
