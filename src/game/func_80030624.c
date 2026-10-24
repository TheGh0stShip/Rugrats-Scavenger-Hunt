/* Target: func_80030624  VRAM 0x80030624  ROM 0x31224
 * Size: 0xA0  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern void func_800306C4(void);
extern void func_800306CC(void);
extern void func_800306FC(void);
extern void (*D_80059DA4)(void);
extern void (*D_80059D9C)(void);
extern void (*D_80059D98)(void);
extern s32 D_80052F18;
extern void func_8003387C(void);
extern void func_8001F30C(void);
extern void func_8000D970(s32);
extern void func_8002673C(void);
extern void func_8002BC6C(void);
extern void func_8001BDD0(void);
extern void func_80026310(s32);

void func_80030624(void) {
    D_80059DA4 = func_800306C4;
    D_80059D9C = func_800306CC;
    D_80059D98 = func_800306FC;
    func_8003387C();
    func_8001F30C();
    func_8000D970(0);
    func_8002673C();
    if (D_80052F18 == 2) {
        func_8002BC6C();
    } else {
        func_8001BDD0();
        func_80026310(D_80052F18);
    }
}
