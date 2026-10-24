/* Target: func_80019740  VRAM 0x80019740  ROM 0x1A340
 * Size: 0x48  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern s32 D_80052F18;
extern s32 D_80053414[2];

s32 func_80019740(s32 arg0) {
    s32 ret = 0;

    if (D_80052F18 == 2) {
        return 1;
    }
    if (arg0 == D_80053414[0] || arg0 == D_80053414[1]) {
        ret = 1;
    }
    return ret;
}
