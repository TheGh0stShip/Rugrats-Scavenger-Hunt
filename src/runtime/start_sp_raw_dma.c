typedef unsigned int u32;

extern char D_800039E0[];
extern char D_800039E4[];
extern void func_8004B870(char *expression, char *file, int line);
extern int func_80051F80(void);
extern u32 func_800495D0(void *address);

int func_80051FE0(int direction, u32 device_address,
                   void *dram_address, u32 size)
{
    if (device_address & 7) {
        func_8004B870(D_800039E0, D_800039E4, 58);
    }
    if ((u32)dram_address & 7) {
        func_8004B870(D_800039E0, D_800039E4, 59);
    }
    if (size & 7) {
        func_8004B870(D_800039E0, D_800039E4, 60);
    }

    if (func_80051F80()) {
        return -1;
    }

    *(volatile u32 *)0xA4040000 = device_address;
    *(volatile u32 *)0xA4040004 = func_800495D0(dram_address);

    if (direction == 0) {
        *(volatile u32 *)0xA404000C = size - 1;
    } else {
        *(volatile u32 *)0xA4040008 = size - 1;
    }

    return 0;
}
