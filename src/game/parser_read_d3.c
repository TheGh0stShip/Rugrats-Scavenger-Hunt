typedef unsigned char u8;
u8 *func_8003EACC(void *object, u8 *input) {
    u8 value = *input;
    ((u8 *)object)[0xD2] = 0;
    ((u8 *)object)[0xD3] = value;
    return input + 1;
}
