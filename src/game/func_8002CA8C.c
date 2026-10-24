/* Target: func_8002CA8C  VRAM 0x8002CA8C  ROM 0x2D68C
 * Size: 0x1A8  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef signed char s8;
typedef int s32;
typedef unsigned int u32;

extern void *D_80052F04;
extern s8 D_800581E9;
extern u32 func_8001BF3C(void);
extern s32 func_80012CE8(void);
extern void func_8002A5EC(s32 value);
extern s32 func_8002A604(void);
extern void func_8000B8A8(s32 a);
extern void func_8000B904(void);
extern void func_80012D9C(s32 a);
extern void func_80012D7C(s32 a);
extern void func_8000E630(void *p);
extern void func_80010AE8(void *p);
extern void func_80019BD4(s32 a);
extern void func_8002E8F0(void);
extern void func_8002EBC4(void);
extern void func_8002EBF0(void);
extern void func_8001A5C0(void);
extern s32 func_8002FEB4(void);

void func_8002CA8C(void) {
    s32 s;

    switch (func_8001BF3C()) {
        case 0x2A:
            if (func_80012CE8() == 1) {
                func_8002A5EC(0x2B);
                func_8000B8A8(0x20000000);
                func_80012D9C(10);
                func_8000E630(D_80052F04);
                func_80019BD4(D_800581E9 + 0x56);
                func_8002E8F0();
            }
            break;
        case 0x2B:
            if (func_80012CE8() == 1) {
                func_8002A5EC(0);
            }
            break;
        case 0x2C:
            if (func_80012CE8() == 1) {
                func_8002A5EC(0x2D);
                func_8000B904();
                func_80012D9C(10);
                func_80010AE8(D_80052F04);
                func_8002EBC4();
            }
            break;
        case 0x2D:
            if (func_80012CE8() == 1) {
                func_8001A5C0();
            }
            break;
    }
    s = func_8002A604();
    if (s != 0x2A && s != 0x2D) {
        func_8002EBF0();
    }
    if (func_8002A604() == 0 && func_8002FEB4() == 0x17) {
        func_8002A5EC(0x2C);
        func_80012D7C(10);
    }
}
