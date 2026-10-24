typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern s32 D_80057D5C;
extern void *func_800055A4(s32);

void *func_80020680(void)
{
    return func_800055A4(D_80057D5C);
}

