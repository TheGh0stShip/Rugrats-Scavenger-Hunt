/* Target: func_8000CF10  VRAM 0x8000CF10  ROM 0xDB10
 * Size: 0x54  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

extern void func_80047A38(void *mf, f32 a1, f32 a2, f32 a3, f32 f0, f32 f1, f32 f2, f32 f3);
extern void func_80047E00(void *mf, s32 flags);

void func_8000CF10(void *mf, f32 a1, f32 a2, f32 a3, f32 f0, f32 f1, f32 f2, f32 f3) {
    func_80047A38(mf, a1, a2, a3, f0, f1, f2, f3);
    func_80047E00(mf, 0x40);
}
