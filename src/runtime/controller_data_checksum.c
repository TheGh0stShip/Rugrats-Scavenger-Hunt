typedef unsigned char u8;

u8 func_8004EDE8(u8 *data) {
    u8 checksum = 0;
    u8 polynomial;
    int i;
    int j;

    for (i = 0; i <= 32; i++, data++) {
        for (j = 7; j >= 0; j--) {
            polynomial = (checksum & 0x80) ? 0x85 : 0;
            checksum <<= 1;
            if (i == 32) {
                checksum |= 0;
            } else {
                checksum |= ((*data & (1 << j)) ? 1 : 0);
            }
            checksum ^= polynomial;
        }
    }
    return checksum;
}
