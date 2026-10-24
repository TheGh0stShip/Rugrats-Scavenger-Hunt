/* Target: func_8001ADC8  VRAM 0x8001ADC8  ROM 0x1B9C8
 * Size: 0x220  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed int s32;

struct Cam_80052F04 {
    u8 pad_00[0xF0];
    s32 unkF0;   /* 0xF0 */
};

extern struct Cam_80052F04 *D_80052F04;
extern void *D_800529F0;
extern s32 D_80053638;
extern s32 func_8001BF3C(void);
extern s32 func_8001BF00(void);
extern void func_8001BF24(s32);
extern void func_8001BED8(s32);
extern s32 func_80012CE8(void);
extern void func_8000B8A8(s32);
extern void func_8000B904(void);
extern void func_80012D9C(s32);
extern void func_80012D7C(s32);
extern void func_8000E630(struct Cam_80052F04 *);
extern void func_8001082C(struct Cam_80052F04 *);
extern void *func_800197C8(s32);
extern void func_8001DEF8(void);
extern void func_8001DEE8(void);
extern void func_80005500(s32, void *);
extern void func_80019E3C(void);
extern s32 func_8001C36C(void);

void func_8001ADC8(void) {
    s32 st;

    switch (func_8001BF3C()) {
        case 0x1B:
            if (func_80012CE8() == 1) {
                func_8001BF24(0x1C);
                func_8000B8A8(0x10000000);
                func_80012D9C(10);
                func_8000E630(D_80052F04);
                D_80052F04->unkF0 = 0x11;
                func_800197C8(0xEA);
                func_8001DEF8();
            }
            break;
        case 0x1C:
            if (func_80012CE8() == 1) {
                func_8001BF24(0);
            }
            break;
        case 0x1D:
            if (func_80012CE8() == 1) {
                func_8001BF24(0x1E);
                func_8000B904();
                func_80012D9C(10);
                func_8001082C(D_80052F04);
                func_8001DEE8();
            }
            break;
        case 0x1E:
            if (func_80012CE8() == 1) {
                if (func_8001BF00() == 0xC) {
                    func_8001BED8(4);
                } else {
                    func_8001BED8(0);
                    func_80005500(6, D_800529F0);
                }
                D_80053638 = 0;
                func_8001BF24(0);
                return;
            }
            break;
    }
    st = func_8001BF3C();
    if (st != 0x1B && st != 0x1E) {
        func_80019E3C();
    }
    if ((func_8001BF00() == 0xC || func_8001BF00() == 0xD) && func_8001BF3C() == 0 &&
        func_8001C36C() != 0) {
        func_8001BF24(0x1D);
        func_80012D7C(10);
    }
}
