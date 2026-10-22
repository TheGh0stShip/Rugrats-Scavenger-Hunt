typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
void *func_8003EB58(void *object, u8 *input) {
    u8 *bytes = object;
    u16 first = (*input++ << 8) | *input++;
    u16 second = (*input++ << 8) | *input++;
    u16 third = (input[0] << 8) | input[1];
    *(u16 *)(bytes + 0xA2) = 1;
    *(u32 *)(bytes + 0x38) = *(u32 *)(bytes + 0x8C) + second;
    *(u16 *)(bytes + 0xA4) = 1;
    *(u32 *)(bytes + 0x34) = *(u32 *)(bytes + 0x88) + third;
    return (void *)(*(u32 *)(bytes + 0x80) + first);
}
