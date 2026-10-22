typedef unsigned char u8;

extern u8 D_8005368C[];

void func_8001BD8C(void) {
    u8 *base = D_8005368C;

    __asm__("" : "+r"(base));
    *(int *)(base + 0x70) = 0x1B1;
    *(int *)(base + 0xAC) = 0x1B4;
    *(int *)(base + 0x34) = 0x1AF;
    *(int *)(base + 0xE8) = 0x1AE;
    *(int *)(base + 0x124) = 0x1AD;
}
