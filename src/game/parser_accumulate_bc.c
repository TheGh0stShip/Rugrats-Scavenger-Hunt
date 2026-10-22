typedef unsigned char u8;
extern int func_8003DBE8(u8 value);
u8 *func_8003EC28(void *object, u8 *input) {
    int value = func_8003DBE8(*input++);
    ((u8 *)object)[0xBC] = value;
    ((u8 *)object)[0xBC] = value + *input;
    return input + 1;
}
