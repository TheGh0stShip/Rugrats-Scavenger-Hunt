typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern s32 D_80052F18;
extern s32 func_800222E8(void *);
extern s32 func_800226A8(void *);

s32 func_800222A8(void *a)
{
    s32 result;
    if (D_80052F18 != 2) {
        result = func_800222E8(a);
    } else {
        result = func_800226A8(a);
    }
    return result;
}

