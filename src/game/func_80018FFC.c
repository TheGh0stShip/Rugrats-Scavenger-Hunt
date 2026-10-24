/* Target: func_80018FFC  VRAM 0x80018FFC  ROM 0x19BFC
 * Size: 0x70  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct State_80052F18 {
    u32 words[13];
};

extern struct State_80052F18 D_80052F18;
extern struct State_80052F18 D_80052F4C;
extern s32 D_80059D40;
extern void func_800196A4(void);

void func_80018FFC(void) {
    func_800196A4();
    D_80059D40 = 8;
    D_80052F4C = D_80052F18;
}
