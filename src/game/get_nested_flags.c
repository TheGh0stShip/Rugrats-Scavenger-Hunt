unsigned int func_8000CCC8(void *object, unsigned int mask) {
    unsigned int *nested;
    register unsigned int flags __asm__("$2");

    nested = *(unsigned int **)((unsigned char *)object + 0x18);
    if (nested == 0) {
        return (unsigned int)nested;
    }
    flags = nested[2];
    return flags & mask;
}
