typedef unsigned int u32;

u32 func_80003A90(volatile u32 *address) {
    while (*(volatile u32 *)0xA4600010 & 3) {
    }
    return *address;
}
