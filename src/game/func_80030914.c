/* Target: func_80030914  VRAM 0x80030914  ROM 0x31514
 * Size: 0x88  Compiler: KMC GCC 2.7.2 (-O2)
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
extern void func_80014F00(void);
extern void func_80015740(void);
extern void func_8003376C(void);
extern void func_80013B14(void);
extern s32 func_8001EFC4(void);
extern s32 func_8001EEAC(void);
extern void func_8001ECE4(s32, s32);

void func_80030914(void) {
    if (D_800B9344 == 1 || D_800B9344 == 7) {
        func_80014F00();
    } else {
        func_80015740();
    }
    func_8003376C();
    func_80013B14();
    if (func_8001EFC4() == 0) {
        if (func_8001EEAC() != 0x205) {
            func_8001ECE4(0x205, 0x18);
        }
    }
}
