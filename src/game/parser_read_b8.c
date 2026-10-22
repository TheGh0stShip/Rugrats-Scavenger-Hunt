typedef unsigned char u8;
u8 *func_8003E520(void *object, u8 *input) {
    u8 value = *input++;
    ((u8 *)object)[0xB8] = value;
    if (value != 0) *(float *)((u8 *)object + 0x50) = *(float *)((u8 *)object + 0x2C);
    return input;
}
