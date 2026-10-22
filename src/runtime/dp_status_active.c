typedef unsigned int u32;

int func_80051D20(void) {
    u32 status = *(volatile u32 *)0xA410000C;

    if ((status & 0x100) != 0) {
        return 1;
    }
    return 0;
}
