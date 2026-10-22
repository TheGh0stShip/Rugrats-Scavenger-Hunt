typedef unsigned char u8;
typedef unsigned int u32;

extern u8 *D_80053634;

void func_8001BF24(u32 value) {
    if (D_80053634 != 0) {
        *(u32 *)(D_80053634 + 0x2C) = value;
    }
}
