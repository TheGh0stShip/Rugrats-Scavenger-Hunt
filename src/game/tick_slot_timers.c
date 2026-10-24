typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
extern s32 D_80052C0C[4];
extern void func_80010EF8(s32 slot);
void func_80010DC0(void) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (D_80052C0C[i] != 0) {
            if (--D_80052C0C[i] == 0) {
                func_80010EF8(i);
            }
        }
    }
}
