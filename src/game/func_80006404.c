/* Target: func_80006404  VRAM 0x80006404  ROM 0x7004
 * Size: 0x64  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

typedef struct Actor_80006404 {
    /* 0x000 */ u8 pad_000[0x118];
    /* 0x118 */ u32 flags;
    /* 0x11C */ s32 unk_11C;
} Actor_80006404;

extern s32 D_800529F8;
extern s16 D_80057DA0;

extern void func_80005500(s32 a, Actor_80006404 *p);
extern void func_8000F150(Actor_80006404 *p);
extern void func_80010E24(s32 a, s32 b);

void func_80006404(Actor_80006404 *p) {
    func_80005500(4, p);
    p->flags |= 0x80;
    func_8000F150(p);
    D_800529F8 = 0;
    D_80057DA0 = 0;
    if (p->unk_11C >= 0) {
        func_80010E24(p->unk_11C, 5);
    }
}
