/* Target: func_8001906C  VRAM 0x8001906C  ROM 0x19C6C
 * Size: 0xD4  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: medium
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

extern u8 D_80059DD0[];
extern u8 D_80052C00;
extern u8 D_80052C01;
extern s32 D_8005496C[];
extern s32 D_8005497C[];
extern s32 D_80059F20;
extern struct State_80052F18 D_80052F18;
extern struct State_80052F18 D_80052F4C;
extern void *func_80038E90(void *, s32, s32);
extern void func_80012264(void);
extern void func_8000B8A8(s32);

void func_8001906C(void) {
    struct State_80052F18 unused;
    s32 bits;
    s32 *src;
    s32 *dst;

    func_80038E90(D_80059DD0, -1, 0xF0);
    func_80012264();
    bits = D_80052C00;
    if (bits != 0) {
        src = D_8005496C;
        dst = &D_8005497C[2];
        do {
            if (bits & 1) {
                *dst = *src;
            }
            bits >>= 1;
            dst++;
            src++;
        } while (bits != 0);
    }
    D_80059F20 = D_80052C01 + 2;
    func_8000B8A8(0);
    D_80052F18 = D_80052F4C;
}
