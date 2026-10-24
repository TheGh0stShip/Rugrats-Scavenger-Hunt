typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_80052E90;
extern s32 D_80052E98;
extern s32 D_80058BF0;
extern char D_800010E0[];

extern int func_8001F588(s32);
extern void func_800137E8(s32);
extern void func_80013724(void);
extern void func_800138FC(char *, s32, s32);
extern void func_800122A4(void);

void func_800121D0(void) {
    u8 unused[0x18];

    if (D_80052E90 != 0) {
        func_8001F588(0);
        if (--D_80058BF0 == 0) {
            D_80058BF0 = 10;
            D_80052E98 ^= 1;
        }
        if (D_80052E98 != 0) {
            func_800137E8(8);
            func_80013724();
            func_800138FC(D_800010E0, 0x100, 0x78);
        }
        func_800122A4();
    }
}
