/* Target: func_8003295C  VRAM 0x8003295C  ROM 0x3355C
 * Size: 0x68  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef signed short s16;
typedef signed int s32;

typedef struct Text_800328CC Text_800328CC;
extern s32 func_800328CC(Text_800328CC *t);
extern void func_80032A20(Text_800328CC *t, s32 x, s32 y);

void func_8003295C(Text_800328CC *t, s16 x, s16 y) {
    func_80032A20(t, x - func_800328CC(t) / 2, y);
}

