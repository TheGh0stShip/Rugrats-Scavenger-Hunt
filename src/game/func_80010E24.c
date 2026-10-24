/* Target: func_80010E24  VRAM 0x80010E24  ROM 0x11A24
 * Size: 0x4C  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Unresolved: element type of D_80052C0C (per-channel timer) guessed s32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 func_80010E70(s32 channel);
extern s32 D_80052C0C[4];

void func_80010E24(s32 channel, s32 value) {
    if (func_80010E70(channel) == 0) {
        D_80052C0C[channel] = value;
    }
}
