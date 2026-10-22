typedef unsigned char u8;

extern u8 *D_80061D78;
extern u8 D_8005EB78[];
extern u8 *D_8005B974;

void func_8001E5F0(void) {
    if (D_80061D78 < D_8005EB78) {
        D_80061D78 = D_8005EB78;
    } else {
        D_80061D78 = D_8005EB78 - 0x3200;
    }
    D_8005B974 = D_80061D78 + 0x3200;
}
