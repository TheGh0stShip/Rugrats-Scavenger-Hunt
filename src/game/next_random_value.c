typedef signed int s32;

extern s32 D_800C3FA0;

s32 func_8003A840(void) {
    s32 product = D_800C3FA0 * 0x41C64E6D;

    D_800C3FA0 = product + 0x3039;
    return ((product + 0x303A) >> 16) & 0x7FFF;
}
