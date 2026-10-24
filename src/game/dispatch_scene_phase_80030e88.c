typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_800545E4;
extern void func_80030F14(void);
extern void func_80030F74(void);
extern void func_80031110(void);
extern void func_800311E8(void);
extern void func_8003121C(void);

void func_80030E88(void) {
    switch (D_800545E4) {
        case 0:
            func_80030F14();
            break;
        case 1:
            func_80030F74();
            break;
        case 2:
            func_80031110();
            break;
        case 3:
            func_800311E8();
            break;
        case 4:
            func_8003121C();
            break;
    }
}
