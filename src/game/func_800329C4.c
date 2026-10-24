/* Target: func_800329C4  VRAM 0x800329C4  ROM 0x335C4
 * Size: 0x5C  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef signed short s16;
typedef signed int s32;

typedef struct Text_800328CC Text_800328CC;
extern s32 func_800328CC(Text_800328CC *t);
extern void func_80032A20(Text_800328CC *t, s32 x, s32 y);

void func_800329C4(Text_800328CC *t, s16 x, s16 y) {
    func_80032A20(t, x - func_800328CC(t), y);
}

