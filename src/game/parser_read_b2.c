typedef unsigned char u8;
typedef unsigned short u16;
u8 *func_8003E76C(void *object, u8 *input) {
    u16 value = *input++ << 8;
    value |= *input;
    *(u16 *)((u8 *)object + 0xB4) = 0;
    *(u16 *)((u8 *)object + 0xB2) = value;
    return input + 1;
}
