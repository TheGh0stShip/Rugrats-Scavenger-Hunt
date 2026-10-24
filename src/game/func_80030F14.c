/* Target: func_80030F14  VRAM 0x80030F14  ROM 0x31B14
 * Size: 0x60  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_800547B0;
extern s32 D_800545E4;
extern void func_80019BD4(s32);

void func_80030F14(void) {
    s32 i;

    for (i = 0; i < 0x211; i++) {
        func_80019BD4(i);
    }
    if (--D_800547B0 == 0) {
        D_800545E4 = 1;
        D_800547B0 = 4;
    }
}
