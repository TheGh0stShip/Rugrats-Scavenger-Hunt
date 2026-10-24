typedef unsigned int u32;
typedef int s32;

extern char D_80003930[];
extern char D_80003934[];
extern void func_8004B870(char *category, char *file, s32 line);
extern s32 func_8004DAA0(void);

s32 func_8004DAC0(u32 address, u32 value)
{
    if (address & 3) {
        func_8004B870(D_80003930, D_80003934, 52);
    }
    if (func_8004DAA0()) {
        return -1;
    }
    *(volatile u32 *)(address | 0xA0000000) = value;
    return 0;
}
