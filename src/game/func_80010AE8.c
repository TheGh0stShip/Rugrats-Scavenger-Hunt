/* Target: func_80010AE8  VRAM 0x80010AE8  ROM 0x116E8
 * Size: 0x24  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Unresolved: none */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern void func_8000E680(void *);
extern s32 D_80057D94;

void func_80010AE8(void *obj) {
    D_80057D94 = 0;
    func_8000E680(obj);
}
