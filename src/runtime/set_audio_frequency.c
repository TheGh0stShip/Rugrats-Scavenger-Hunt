typedef signed int s32;
typedef unsigned char u8;
typedef unsigned int u32;

extern s32 D_80056048;

s32 func_8004D2B0(u32 frequency) {
    u32 dac_rate;
    u8 bit_rate;
    float value;

    value = D_80056048 / (float)frequency + 0.5f;
    dac_rate = value;
    if (dac_rate < 132) {
        return -1;
    }
    bit_rate = dac_rate / 66;
    if (bit_rate > 16) {
        bit_rate = 16;
    }
    *(volatile u32 *)0xA4500010 = dac_rate - 1;
    *(volatile u32 *)0xA4500014 = bit_rate - 1;
    *(volatile u32 *)0xA4500008 = 1;
    return D_80056048 / (s32)dac_rate;
}
