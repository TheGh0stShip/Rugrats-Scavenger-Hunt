/* func_80031110: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_800545E4;
extern s32 D_800B9338;
extern s32 D_80052F18;
extern s32 D_800535E0;
extern char D_80002B34[];
extern char D_80002B40[];
extern u32 func_80016530(s32);
extern s32 func_8001E2B0(void);
extern void func_8001E0E0(void);
extern s32 func_8001E168(void);
extern int func_8001F588(s32);
extern void func_800139F8(char *, s32, s32);

void func_80031110(void) {
    s32 i;
    u32 bits;

    i = 0;
    do {
        bits = func_80016530(i) & 0x10;
        i++;
    } while (i < 4 && bits == 0);
    if (bits != 0 || func_8001E2B0() != 0) {
        func_8001E0E0();
        D_800545E4 = 3;
        D_800B9338 = 3;
    } else if (D_80052F18 == 0 && func_8001E168() < 200) {
        func_8001F588(0);
        if (D_800535E0 == 1) {
            func_800139F8(D_80002B34, 0x100, 200);
        } else {
            func_800139F8(D_80002B40, 0x100, 200);
        }
    }
}
