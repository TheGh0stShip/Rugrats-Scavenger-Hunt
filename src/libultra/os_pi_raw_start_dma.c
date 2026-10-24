typedef unsigned int u32;
typedef int s32;

extern u32 D_80000308;

extern u32 func_800495D0(void *address);

s32 func_8004FA20(s32 direction, u32 device_address, void *dram_address, u32 size)
{
    register u32 status;

    status = *(volatile u32 *)0xA4600010;
    while (status & 3) {
        status = *(volatile u32 *)0xA4600010;
    }

    *(volatile u32 *)0xA4600000 = func_800495D0(dram_address);
    *(volatile u32 *)0xA4600004 = (D_80000308 | device_address) & 0x1FFFFFFF;

    switch (direction) {
    case 0:
        *(volatile u32 *)0xA460000C = size - 1;
        break;
    case 1:
        *(volatile u32 *)0xA4600008 = size - 1;
        break;
    default:
        return -1;
    }

    return 0;
}
