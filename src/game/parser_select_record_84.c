typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
u8 *func_8003EAFC(void *object, u8 *input) {
    u8 *bytes = object;
    u16 index = *input++;
    if (index >= 0x80) index = ((index & 0x7F) << 8) | *input++;
    *(u32 *)(bytes + 0x84) = *(u32 *)(*(u32 *)(bytes + 0x74) + 0x1C) + index * 6;
    return input;
}
