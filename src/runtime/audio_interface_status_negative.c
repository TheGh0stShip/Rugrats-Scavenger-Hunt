typedef unsigned int u32;

int func_8004D480(void) {
    u32 status = *(volatile u32 *)0xA450000C;

    if ((status & 0x80000000) != 0) {
        return 1;
    }
    return 0;
}
