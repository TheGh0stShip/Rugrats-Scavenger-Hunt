typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

extern s32 D_80053984;
extern s32 func_8003BD20(s32, s32, s32, s32, s32);

s32 func_8001F170(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (D_80053984 == 0) {
        D_80053984 = 1;
    }
    if (arg0 == 0 || arg0 >= 0x5C) {
        return 0;
    }
    return func_8003BD20(arg0, arg1 * 2, 0x80, arg3, -1);
}

