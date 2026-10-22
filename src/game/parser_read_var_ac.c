typedef unsigned char u8;
typedef unsigned short u16;
u8 *func_8003E884(void *object, u8 *input) {
    int value = *input++;
    if (value >= 0x80) {
        value &= 0x7F;
        value <<= 8;
        value |= *input++;
    }
    *(u16 *)((u8 *)object + 0xAC) = value;
    return input;
}
