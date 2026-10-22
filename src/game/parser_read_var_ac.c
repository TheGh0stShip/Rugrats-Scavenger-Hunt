typedef unsigned char u8;
typedef unsigned short u16;
u8 *func_8003E884(void *object, u8 *input) {
    u16 value = *input++;
    if (value >= 0x80) value = ((value & 0x7F) << 8) | *input++;
    *(u16 *)((u8 *)object + 0xAC) = value;
    return input;
}
