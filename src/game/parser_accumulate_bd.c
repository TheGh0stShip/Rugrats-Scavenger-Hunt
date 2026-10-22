typedef unsigned char u8;
extern int func_8003DBE8(u8 value);
u8 *func_8003EC78(void *object, u8 *input) {
    int value = func_8003DBE8(*input++);
    ((u8 *)object)[0xBD] = value;
    ((u8 *)object)[0xBD] = value + *input;
    return input + 1;
}
