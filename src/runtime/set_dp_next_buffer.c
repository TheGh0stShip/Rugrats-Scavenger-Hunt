typedef unsigned int u32;
typedef unsigned long long u64;

extern int func_80051D20(void);
extern u32 func_800495D0(void *address);

int func_80051C80(void *buffer, u64 size) {
    u32 status;

    if (func_80051D20() != 0) {
        return -1;
    }

    *(volatile u32 *)0xA410000C = 1;
    while (1) {
        status = *(volatile u32 *)0xA410000C;
        if ((status & 1) == 0) {
            break;
        }
    }
    *(volatile u32 *)0xA4100000 = func_800495D0(buffer);
    *(volatile u32 *)0xA4100004 = func_800495D0(buffer) + size;
    return 0;
}
