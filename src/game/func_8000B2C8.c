/* Target: func_8000B2C8  VRAM 0x8000B2C8  ROM 0xBEC8  Size: 0xAC
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern s32 D_80052F18;
extern s32 D_80059D40;
extern s32 func_8001BF00(void);
extern void func_8001BED8(s32 arg);
extern void func_8001BF24(s32 arg);
extern s32 func_8000AE0C(void);
extern s32 func_8000B00C(void);
extern void func_80012D9C(s32 arg);

void func_8000B2C8(void) {
    s32 done = 0;

    if (func_8001BF00() == 0x31) {
        func_8001BED8(0x32);
        func_8001BF24(1);
    }
    if (D_80052F18 == 0) {
        done = 1;
    } else if (D_80052F18 == 1) {
        done = func_8000AE0C();
    } else if (D_80052F18 == 2) {
        done = func_8000B00C();
    }
    if (done) {
        func_80012D9C(10000);
        func_8001BED8(0);
        D_80059D40 = 6;
    }
}
