typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_80059D40;
extern u16 D_80058B5A;
extern s32 D_800B9330;
extern s32 D_800545C0;
extern s32 D_800545C4[];
extern s32 D_800545E8;
extern int func_8001F588(s32);
extern void func_80013724(void);
extern s32 func_80030238(void);
extern void func_80012D7C(s32);
extern s32 func_80012CE8(void);
extern void func_80012D9C(s32);

void func_800304A4(void) {
    func_8001F588(0);
    func_80013724();
    switch (func_80030238()) {
        case 0: break;
        case 1:
            if (D_80059D40 == 9 || D_80058B5A == 0) {
                break;
            }
            D_800B9330 = 0;
        case 2:
            func_80012D7C(0xF);
            break;
        case 3:
            if (func_80012CE8() == 1) {
                D_800545C0++;
                D_80059D40 = D_800545C4[D_800545C0];
                func_80012D9C(1);
                if (D_800545E8 != 0) {
                    D_80059D40 = 7;
                }
            }
            break;
    }
}
