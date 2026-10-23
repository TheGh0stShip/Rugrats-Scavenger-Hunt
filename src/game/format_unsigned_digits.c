typedef unsigned char u8;
typedef unsigned int u32;

extern u8 *D_800C3E70;
extern u8 D_800C3FA0[];

u8 *func_80039024(u32 value, u32 radix) {
    u8 *output = D_800C3FA0;
    do {
        *--output = D_800C3E70[value % radix];
        value /= radix;
    } while (value != 0);
    return output;
}
