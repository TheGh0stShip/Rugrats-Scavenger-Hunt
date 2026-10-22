typedef unsigned int u32;
void func_800137F4(u32 mask) {
    register unsigned char *base __asm__("$3") = (unsigned char *)0x80050000;
    __asm__("" : "=r"(base) : "0"(base));
    *(u32 *)(base + 0x2ed0) &= ~mask;
}
