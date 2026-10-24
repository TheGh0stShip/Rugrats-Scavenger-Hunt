typedef int s32;
typedef unsigned int u32;

extern char D_80003950;
extern char D_80003954;

extern void func_8004B870(char *expression, char *file, s32 line);
extern s32 func_8004DAA0(void);
extern void func_80047E00(void *address, s32 size);
extern void func_80047C50(void *address, s32 size);
extern u32 func_800495D0(void *address);

s32 func_8004DD10(s32 direction, void *dram_address) {
    if (((u32)dram_address & 3) != 0) {
        func_8004B870(&D_80003950, &D_80003954, 55);
    }
    if (func_8004DAA0()) {
        return -1;
    }
    if (direction == 1) {
        func_80047E00(dram_address, 0x40);
    }
    *(volatile u32 *)0xA4800000 = func_800495D0(dram_address);
    if (direction == 0) {
        *(volatile u32 *)0xA4800004 = 0x1FC007C0;
    } else {
        *(volatile u32 *)0xA4800010 = 0x1FC007C0;
    }
    if (direction == 0) {
        func_80047C50(dram_address, 0x40);
    }
    return 0;
}
