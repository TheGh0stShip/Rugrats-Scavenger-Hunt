extern int D_80061D9C;
extern int D_80061DA0;
void func_8001E650(void) {
    unsigned char *base = (unsigned char *)0x80060000;

    __asm__("" : "=r"(base) : "0"(base));
    base += 0x1D90;
    *(int *)(base + 0xC) = -1;
    *(int *)(base + 0x10) = -1;
}
