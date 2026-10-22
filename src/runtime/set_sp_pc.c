typedef unsigned int u32;

int func_80051FB0(u32 address) {
    if ((*(volatile u32 *)0xA4040010 & 1) != 0) {
        *(volatile u32 *)0xA4080000 = address;
        return 0;
    }
    return -1;
}
