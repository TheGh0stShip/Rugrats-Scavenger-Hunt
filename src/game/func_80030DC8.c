/* Target: func_80030DC8  VRAM 0x80030DC8  ROM 0x319C8
 * Size: 0x64  Compiler: KMC GCC 2.7.2 (-O2)
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
extern s32 D_80052F18;
extern void func_80019C9C(void);
extern void func_80018FFC(void);
extern void func_8001F30C(void);
extern void func_8001E6F8(s32);

void func_80030DC8(void) {
    if (--D_800B9338 == -1) {
        if (D_80052F18 != 2) {
            func_80019C9C();
        }
        func_80018FFC();
        func_8001F30C();
    }
    func_8001E6F8(0xF);
}
