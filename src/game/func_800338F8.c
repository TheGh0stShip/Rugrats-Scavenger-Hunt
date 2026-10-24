/* Target: func_800338F8  VRAM 0x800338F8  ROM 0x344F8
 * Size: 0x78  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef signed int s32;

extern void *func_8001A2B0(s32 size);
extern void *func_80038E90(void *p, s32 v, s32 n);
extern void *D_800581B4;
extern void *D_800581B8;
extern void *D_800581B0;

void func_800338F8(void) {
    D_800581B4 = func_8001A2B0(0x3800);
    func_80038E90(D_800581B4, 0, 0x3800);
    D_800581B8 = func_8001A2B0(0x3800);
    func_80038E90(D_800581B8, 0, 0x3800);
    D_800581B0 = func_8001A2B0(0x3800);
    func_80038E90(D_800581B0, 0, 0x3800);
}
