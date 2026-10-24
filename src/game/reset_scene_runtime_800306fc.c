typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_80059DA8;
extern s32 D_80052EE8;
extern u32 D_80052EE0;
extern void func_80008240(s32);
extern void func_80019BD4(s32);
extern void func_80019D44(void);
extern void func_8000E17C(void);
extern void func_8001F30C(void);
extern void func_80027FD4(void);
extern s32 func_80012488(void);
extern void func_80028E9C(void);

void func_800306FC(void) {
    s32 i;

    D_80052EE8 = 0;
    func_80008240(D_80059DA8);
    for (i = 0; i < 0x211; i++) {
        func_80019BD4(i);
    }
    func_80019D44();
    func_8000E17C();
    func_8001F30C();
    func_80027FD4();
    D_80052EE0 &= ~1;
    D_80052EE0 &= ~2;
    if (func_80012488() != 0) {
        func_80028E9C();
    }
}
