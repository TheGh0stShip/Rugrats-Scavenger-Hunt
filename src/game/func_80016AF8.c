/* Target: func_80016AF8  VRAM 0x80016AF8  ROM 0x176F8
 * Size: 0x8C  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Pos_800533E4 {
    u8 pad_00[0x4];
    s16 x;
    s16 y;
};
extern struct Pos_800533E4 D_800533E4[];
extern void func_80033144(s32, s32, s32, s32);
extern void *func_80032870(s32);
extern void func_8003295C(void *, s32, s32);

void func_80016AF8(s32 mask, s32 arg1) {
    struct Pos_800533E4 *p;
    s32 i;

    for (i = 0, p = D_800533E4; mask != 0; mask >>= 1, p++, i++) {
        if (mask & 1) {
            func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
            func_8003295C(func_80032870(i + 0x39), p->x, p->y);
        }
    }
}
