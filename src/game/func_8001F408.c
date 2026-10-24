typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

extern u16 D_80053978[4];
extern s32 func_80043554(u16);
extern void func_800435B4(u16, s32, u16);

void func_8001F408(s32 arg0) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (func_80043554(D_80053978[i])) {
            func_800435B4(D_80053978[i], 0x40, arg0);
        }
    }
}

