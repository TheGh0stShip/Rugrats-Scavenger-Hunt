typedef unsigned char u8;
extern int func_8003DBE8(u8 value);
u8 *func_8003EBD8(void *object, u8 *input) {
    int value = func_8003DBE8(*input++);
    ((u8 *)object)[0xB9] = value;
    ((u8 *)object)[0xB9] = value + *input;
    return input + 1;
}
