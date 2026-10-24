/* Target: func_800279B0  VRAM 0x800279B0  ROM 0x285B0
 * Size: 0x6C  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef unsigned char u8;
typedef signed int s32;

extern u8 D_80072160[];
extern u8 D_80076310[];
extern void func_80048AB0(s32);
extern void func_80047EE0(void *, s32, void (*)(s32), s32, void *, s32);
extern void func_80049160(void *);
extern void func_80027A1C(s32);

void func_800279B0(s32 arg0) {
    u8 buf[0x40];

    func_80048AB0(arg0);
    func_80047EE0(D_80072160, 1, func_80027A1C, arg0, D_80076310, 10);
    func_80049160(D_80072160);
}
