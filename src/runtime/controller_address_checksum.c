typedef unsigned char u8;
typedef unsigned short u16;

u8 func_8004EDA0(u16 address) {
    u8 checksum = 0;
    u8 polynomial;
    int i;

    for (i = 0; i < 16; i++) {
        polynomial = (checksum & 0x10) ? 0x15 : 0;
        checksum <<= 1;
        checksum |= (u8)((address & 0x400) ? 1 : 0);
        address <<= 1;
        checksum ^= polynomial;
    }
    return checksum & 0x1F;
}
