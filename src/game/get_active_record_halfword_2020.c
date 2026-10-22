typedef signed short s16;
typedef unsigned char u8;

extern u8 *D_800581F0;

s16 func_8000E008(int index) {
    register int offset __asm__("$3") = index * 0x210;
    register u8 *base __asm__("$2") = D_800581F0;
    base += offset;
    return *(s16 *)(base + 0x2020);
}
