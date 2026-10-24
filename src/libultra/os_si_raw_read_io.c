typedef unsigned int u32;
typedef int s32;

extern char D_80003920[];
extern char D_80003924[];
extern void func_8004B870(char *category, char *file, s32 line);
extern s32 func_8004DAA0(void);

s32 func_8004DA10(u32 address, u32 *value)
{
    if (address & 3) {
        func_8004B870(D_80003920, D_80003924, 51);
    }
    if (value == 0) {
        func_8004B870(D_80003920, D_80003924, 52);
    }
    if (func_8004DAA0()) {
        return -1;
    }
    *value = *(volatile u32 *)(address | 0xA0000000);
    return 0;
}
