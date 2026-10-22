typedef unsigned char u8;

extern u8 *D_8006D680;

void func_8001FF3C(u8 *base) {
    register int row __asm__("$7");
    register int index __asm__("$6");
    register u8 **entry __asm__("$5");
    register int offset __asm__("$3");

    row = 0;
    __asm__("" : "+r"(row));
    D_8006D680 = base;
    do {
        index = 0;
        entry = (u8 **)base;
        offset = 0x20;

        do {
            *entry++ = base + offset;
            offset += 0xE10;
            index++;
        } while (index < 8);
        row++;
        base += 0x70A0;
    } while (row < 2);
}
