/* Target: func_800069C8  VRAM 0x800069C8  ROM 0x75C8
 * Size: 0x118  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

typedef struct Actor_800069C8 {
    /* 0x000 */ u8 pad_000[0xE0];
    /* 0x0E0 */ s32 unk_E0;
    /* 0x0E4 */ u8 pad_0E4[0xF8 - 0xE4];
    /* 0x0F8 */ s32 unk_F8;
    /* 0x0FC */ u8 pad_0FC[0x118 - 0xFC];
    /* 0x118 */ u32 flags;
} Actor_800069C8;

extern s32 D_80052F18;
extern s32 D_80052FFC[];
extern s32 D_8005300C[];

extern u8 *func_800197C8(s32 a);
extern s32 func_80005598(Actor_800069C8 *p);
extern Actor_800069C8 *func_800206A8(void);
extern void func_8000C530(void *a, s32 b, s32 c);
extern void func_8000CCB0(void *a, s32 b);

void func_800069C8(s32 anim, Actor_800069C8 *p, s32 loop) {
    u8 *obj;

    obj = func_800197C8(p->unk_E0) + 0x2C;
    if (func_80005598(p) != 0) {
        if (p->flags & 2) {
            func_8000C530(func_800197C8(func_800206A8()->unk_E0) + 0x2C, anim, loop);
            anim += 8;
        } else if (p->flags & 0x20) {
            obj = func_800197C8(D_80052FFC[p->unk_F8]) + 0x2C;
        } else if (p->flags & 0x10000) {
            obj = func_800197C8(D_8005300C[p->unk_F8]) + 0x2C;
        }
    }
    if (anim >= 15 && D_80052F18 != 2) {
        func_8000CCB0(obj, 8);
    }
    func_8000C530(obj, anim, loop);
}
