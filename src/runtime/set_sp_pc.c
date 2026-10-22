typedef unsigned int u32;

int func_80051FB0(u32 address) {
    register u32 status = *(volatile u32 *)0xA4040010;

    if (!(status & 1)) {
        return -1;
    }
    *(volatile u32 *)0xA4080000 = address;

    return 0;
}
