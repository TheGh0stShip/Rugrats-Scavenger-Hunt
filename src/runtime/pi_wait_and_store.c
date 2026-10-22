typedef unsigned int u32;

void func_80003A58(u32 *destination, u32 value) {
    volatile u32 *piStatus = (volatile u32 *)0xA4600010;
    u32 status = *piStatus;

    if ((status & 3) != 0) {
        do {
            status = *piStatus;
        } while ((status & 3) != 0);
    }
    *destination = value;
}
