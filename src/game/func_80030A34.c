/* Target: func_80030A34  VRAM 0x80030A34  ROM 0x31634
 * Size: 0x80  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_800B9344;
extern s32 D_800B9338;
extern u32 func_80016530(s32);
extern s32 func_8001E2B0(void);
extern void func_8001E0E0(void);

void func_80030A34(void) {
    s32 i;
    u32 bits;

    i = 0;
    do {
        bits = func_80016530(i) & 0x10;
        i++;
    } while (i < 4 && bits == 0);
    if (bits != 0 || func_8001E2B0() != 0) {
        func_8001E0E0();
        D_800B9344 = 2;
        D_800B9338 = 3;
    }
}
