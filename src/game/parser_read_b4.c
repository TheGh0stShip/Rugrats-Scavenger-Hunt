typedef unsigned char u8;
typedef unsigned short u16;
u8 *func_8003E758(void *object, u8 *input) {
    u8 value = *input;
    *(u16 *)((u8 *)object + 0xB2) = 0;
    *(u16 *)((u8 *)object + 0xB4) = value;
    return input + 1;
}
