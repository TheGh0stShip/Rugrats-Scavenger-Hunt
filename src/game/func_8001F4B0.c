typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

extern void *D_8006D690[2];
extern void *D_8006D698[2];
extern void *func_8001A2B0(s32);

void func_8001F4B0(void) {
    s32 i;
    D_8006D690[0] = func_8001A2B0(0xE140);
    D_8006D690[1] = func_8001A2B0(0xE140);
    for (i = 1; i >= 0; i--) {
        D_8006D698[i] = 0;
    }
}

