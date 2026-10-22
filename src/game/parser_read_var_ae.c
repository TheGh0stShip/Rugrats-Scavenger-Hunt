typedef unsigned char u8;
typedef unsigned short u16;
u8 *func_8003E4F0(void *object, u8 *input) {
    u16 value = *input++;
    if (value & 0x80) value = *input++ | ((value & 0x7F) << 8);
    *(u16 *)((u8 *)object + 0xAE) = value;
    return input;
}
