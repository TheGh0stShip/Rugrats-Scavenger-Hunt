/* Target: func_80007EB0  VRAM 0x80007EB0  ROM 0x8AB0
 * Size: 0x3C  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef int s32;

extern u8 *D_80052F04;
extern void *func_8000B910(void *, void *, void *, void *, void *, s32);

void func_80007EB0(void *arg0, void *arg1, void *arg2, void *arg3, s32 arg4) {
    func_8000B910(arg0, arg1, D_80052F04 + 0x28, arg2, arg3, arg4);
}
