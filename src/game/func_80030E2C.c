/* Target: func_80030E2C  VRAM 0x80030E2C  ROM 0x31A2C
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

extern void func_80030E88(void);
extern void func_80030590(void);
extern void func_80031250(void);
extern void (*D_80059DA4)(void);
extern void (*D_80059D9C)(void);
extern void (*D_80059D98)(void);
extern s32 D_800B933C;
extern s32 D_800545E4;
extern void func_80012D9C(s32);

void func_80030E2C(void) {
    D_80059DA4 = func_80030E88;
    D_80059D9C = func_80030590;
    D_80059D98 = func_80031250;
    D_800B933C = 0;
    func_80012D9C(0xF);
    D_800545E4 = 0;
}
