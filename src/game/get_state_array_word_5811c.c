typedef unsigned char u8;
typedef unsigned int u32;

extern u8 *D_80057F70;

u32 func_80009150(int index) {
    return *(u32 *)(D_80057F70 + 0x1AC + index * 4);
}
