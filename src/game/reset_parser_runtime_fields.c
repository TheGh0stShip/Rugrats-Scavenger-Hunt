typedef unsigned char u8;
typedef unsigned int u32;

void func_8003D554(u8 *object) {
    *(u32 *)(object + 0x94) = *(u32 *)(object + 0x40);
    object[0xD9] = 0;
    object[0xDA] = object[0xBD] & 0x40;
}
