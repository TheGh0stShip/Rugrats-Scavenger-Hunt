/* Target: func_80016AA8  VRAM 0x80016AA8  ROM 0x176A8
 * Size: 0x50  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */

typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

s32 func_80016AA8(u8 *arg0) {
    if (arg0[2] == 0) {
        return 0;
    }
    arg0[0] += arg0[3];
    if (--arg0[2] != 0) {
        return 1;
    }
    arg0[0] = arg0[1];
    return 2;
}
