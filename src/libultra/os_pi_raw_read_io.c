typedef unsigned int u32;
typedef int s32;

extern u32 D_80000308;
extern char D_80003960;
extern char D_80003964;

extern void func_8004B870(char *expression, char *file, s32 line);

s32 func_8004F7D0(u32 device_address, u32 *value)
{
    register u32 status;
    u32 address;

    if (value == 0) {
        func_8004B870(&D_80003960, &D_80003964, 61);
    }

    status = *(volatile u32 *)0xA4600010;
    while (status & 3) {
        status = *(volatile u32 *)0xA4600010;
    }
    address = D_80000308 | device_address;
    *value = *(volatile u32 *)(address | 0xA0000000);
    return 0;
}
