/* Target: func_80030CC8  VRAM 0x80030CC8  ROM 0x318C8
 * Size: 0x100  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: medium
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_80052F18;
extern s32 D_800B9344;
extern s32 D_800B9338;
extern u32 func_80016530(s32);
extern s32 func_8001EFC4(void);
extern s32 func_8001EEAC(void);
extern void func_8001ECE4(s32, s32);
extern s32 func_8001E2B0(void);
extern void func_8001E0E0(void);

void func_80030CC8(void) {
    s32 i;
    u32 bits;

    i = 0;
    do {
        bits = func_80016530(i) & 0x10;
        i++;
    } while (i < 4 && bits == 0);
    if (func_8001EFC4() == 0) {
        if (D_80052F18 == 0) {
            if (func_8001EEAC() != 0x201) {
                func_8001ECE4(0x202, 0x18);
            }
        } else if (D_80052F18 == 1) {
            if (func_8001EEAC() != 0x205) {
                func_8001ECE4(0x204, 0x18);
            }
        } else {
            if (func_8001EEAC() != 0x20B) {
                func_8001ECE4(0x20B, 0x18);
            }
        }
    }
    if (bits != 0 || func_8001E2B0() != 0) {
        func_8001E0E0();
        D_800B9344 = 8;
        D_800B9338 = 3;
    }
}
