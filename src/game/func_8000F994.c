/* Target: func_8000F994  VRAM 0x8000F994  ROM 0x10594
 * Size: 0x40  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern void func_8001A3EC(void *obj);
extern void *D_80052BB4;

void func_8000F994(void *obj) {
    func_8001A3EC(obj);
    if (D_80052BB4 != 0) {
        func_8001A3EC(D_80052BB4);
        D_80052BB4 = 0;
    }
}
