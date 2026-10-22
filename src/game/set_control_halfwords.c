typedef signed short s16;
extern s16 D_800C3FEC;
extern s16 D_800C3FEE;
void func_8003B9F4(int mask, s16 value) {
    if (mask & 1) D_800C3FEC = value;
    if (mask & 2) D_800C3FEE = value;
}
