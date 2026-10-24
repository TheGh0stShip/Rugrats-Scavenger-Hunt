typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
extern s32 D_80052E90, D_80058BF4, D_80052E9C;
extern u16 D_80058B5C;
extern void func_80012324(void);
extern void func_80012450(void);
void func_800122A4(void) {
    if (D_80052E90 == 0) {
        if (--D_80058BF4 <= 0) {
            func_80012324();
        }
    } else {
        u16 flag;
        s32 t = D_80058BF4 - 1;
        D_80058BF4 = t;
        flag = D_80058B5C;
        if (t <= 0 || flag) {
            D_80052E9C++;
            func_80012450();
        }
    }
}
