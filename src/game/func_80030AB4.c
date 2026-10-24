/* Target: func_80030AB4  VRAM 0x80030AB4  ROM 0x316B4
 * Size: 0x44  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_800B9338;
extern s32 D_800B9344;
extern void func_8001E6F8(s32);

void func_80030AB4(void) {
    if (--D_800B9338 == -1) {
        D_800B9344 = 3;
    }
    func_8001E6F8(0xF);
}
