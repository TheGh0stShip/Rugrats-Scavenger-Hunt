typedef unsigned char u8;
u8 *func_8003EAE0(void *object, u8 *input) {
    ((u8 *)object)[0xBD] = *input >> 1;
    return input + 1;
}
