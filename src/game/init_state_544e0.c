typedef unsigned char u8;
typedef unsigned int u32;
extern u8 D_800544E0[];
void func_8002BC50(void) {
    u8 *base = D_800544E0;

    __asm__("" : "=r"(base) : "0"(base));
    *(u32 *)(base + 0x70) = 0x1B2;
    *(u32 *)(base + 0x34) = 0x1B3;
}
