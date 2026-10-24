/* Target: func_8001271C  VRAM 0x8001271C  ROM 0x1331C
 * Size: 0x5C  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_80058C00;

extern void func_80019BD4(s32);
extern void func_8001A3EC(void *);
extern void *func_8001EED0(void);

void func_8001271C(void *arg0) {
    func_80019BD4(0x82);
    func_80019BD4(0x83);
    func_80019BD4(0x84);
    func_80019BD4(0x85);
    D_80058C00 = 0;
    func_8001A3EC(arg0);
    func_8001EED0();
}
