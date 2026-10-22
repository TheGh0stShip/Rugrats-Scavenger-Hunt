typedef unsigned char u8;
typedef unsigned int u32;
u8 *func_8003E874(void *object, u8 *input) {
    ((u8 *)object)[0xD5] = 0;
    *(u32 *)((u8 *)object + 0x68) = 0;
    return input;
}
