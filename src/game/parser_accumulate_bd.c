typedef unsigned char u8;
extern int func_8003DBE8(u8 value);
u8 *func_8003EC78(void *object, u8 *input) {
    register int value __asm__("$3") = func_8003DBE8(*input++);
    register int next __asm__("$4");

    __asm__("" : "=r"(value) : "0"(value));
    ((u8 *)object)[0xBD] = value;
    next = *input;
    __asm__("" : "=r"(next) : "0"(next));
    ((u8 *)object)[0xBD] = value + next;
    return input + 1;
}
