/* Target: func_8002C8CC  VRAM 0x8002C8CC  ROM 0x2D4CC
 * Size: 0x1C0  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef signed char s8;
typedef int s32;
typedef unsigned int u32;

extern void *D_80052F04;
extern s8 D_800581E9;
extern u32 func_8001BF3C(void);
extern s32 func_80012CE8(void);
extern void func_8002A5EC(s32 value);
extern void func_8002A5B0(s32 value);
extern s32 func_8002A604(void);
extern s32 func_8002A5C8(void);
extern void func_8000B8A8(s32 a);
extern void func_8000B904(void);
extern void func_80012D9C(s32 a);
extern void func_80012D7C(s32 a);
extern void func_8000E630(void *p);
extern void func_80010AE8(void *p);
extern void func_80019BD4(s32 a);
extern void func_8002EBC4(void);
extern void func_8002E930(void);
extern s32 func_8001C36C(void);
extern s32 func_8001C340(void);

void func_8002C8CC(void) {
    s32 s;

    switch (func_8001BF3C()) {
        case 0x1F:
            if (func_80012CE8() == 1) {
                func_8002A5EC(0x20);
                func_8000B8A8(0x20000000);
                func_80012D9C(10);
                func_8000E630(D_80052F04);
                func_80019BD4(D_800581E9 + 0x56);
            }
            break;
        case 0x20:
            if (func_80012CE8() == 1) {
                func_8002A5EC(0);
            }
            break;
        case 0x21:
            if (func_80012CE8() == 1) {
                func_8002A5EC(0x22);
                func_8000B904();
                func_80012D9C(10);
                func_80010AE8(D_80052F04);
                func_8002EBC4();
            }
            break;
        case 0x22:
            if (func_80012CE8() == 1) {
                func_8002A5B0(4);
                func_8002A5EC(0);
            }
            break;
    }
    s = func_8002A604();
    if (s != 0x1F && s != 0x22) {
        func_8002E930();
    }
    if (func_8002A5C8() == 9 && func_8002A604() == 0) {
        if (func_8001C36C() != 0 || func_8001C340() != 0) {
            func_8002A5EC(0x21);
            func_80012D7C(10);
        }
    }
}
