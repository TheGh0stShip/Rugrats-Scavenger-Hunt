typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

extern u32 D_80000318;
extern u8 D_800575B0;
extern u8 D_800575B2;
extern u32 D_800529A8;
extern u32 D_800529B8;

extern s32 func_800039F0(void);
extern void func_80004BC0(void);
extern void func_80003A58(u32 addr, u32 val);
extern void func_80003A28(s32 mask);

void func_8000438C(void) {
    s32 mask = func_800039F0();
    u32 size;

    func_80004BC0();
    func_80003A58(0xB1FFFFF0, 0);
    func_80003A58(0xB1FFFFFC, 0);
    size = D_80000318;
    D_800575B0 = 1;
    D_800575B2 = 0xFF;
    if (size > 0x3FFFFF) {
        if (((size & 0x1FFFF) == 0) & (size <= 0x2000000)) {
            size--;
            D_800529A8 = size + 0x80000000;
            D_800529B8 = size + 0xA0000000;
        }
    }
    func_80003A28(mask);
}
