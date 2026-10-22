typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
u8 *func_8003EAFC(void *object, u8 *input) {
    u8 *bytes = object;
    int index = *input++;
    if (index >= 0x80) {
        index &= 0x7F;
        index <<= 8;
        index |= *input++;
    }
    *(u32 *)(bytes + 0x84) = *(u32 *)(*(u32 *)(bytes + 0x74) + 0x1C) + index * 6;
    return input;
}
