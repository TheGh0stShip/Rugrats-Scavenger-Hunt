/* Target: func_80030B84  VRAM 0x80030B84  ROM 0x31784
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

extern s32 D_80054640;
extern s32 D_800B9344;
extern void func_8000D768(s32);
extern void func_8001F30C(void);
extern void func_8001E6F8(s32);
void func_80030B84(void) {
    if (--D_80054640 != -1) {
        func_8000D768(8);
        func_8001F30C();
    } else {
        D_800B9344 = 6;
        D_80054640 = 3;
    }
    func_8001E6F8(0xF);
}
