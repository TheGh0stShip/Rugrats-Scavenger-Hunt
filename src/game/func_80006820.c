/* Target: func_80006820  VRAM 0x80006820  ROM 0x7420
 * Size: 0x13C  Pipeline: SN GCC 2.8 -O2 -G0 (normalized return)
 * Confidence: medium */
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

typedef struct Actor_80006820 {
    /* 0x000 */ u8 pad_000[0x118];
    /* 0x118 */ u32 flags;
    /* 0x11C */ u8 pad_11C[0x144 - 0x11C];
    /* 0x144 */ s32 unk_144;
    /* 0x148 */ u8 pad_148[0x150 - 0x148];
    /* 0x150 */ s16 unk_150;
    /* 0x152 */ u8 pad_152[2];
    /* 0x154 */ s16 unk_154;
} Actor_80006820;

extern s16 D_80052B26[];
extern s32 D_800529FC[];

extern s16 func_8000E154(s16 a, s32 b);
extern void func_80005528(Actor_80006820 *p, s32 v);
extern int func_8001F138(s32 a);

void func_80006820(Actor_80006820 *p) {
    s32 idx = -1;
    s32 val = 0;
    u32 t;

    if (p->flags & 0x800) {
        idx = p->unk_154;
        p->flags = (p->flags | 0x10) & ~0x800;
    } else {
        t = func_8000E154(p->unk_150, p->unk_144);
        if (p->unk_150 == 0) {
            switch (t) {
                case 1:
                    idx = 1;
                    break;
                case 2:
                    idx = 2;
                    break;
                case 3:
                    idx = 3;
                    break;
                case 4:
                    idx = 4;
                    break;
            }
        } else if (t == 13) {
            idx = 0;
            val = D_80052B26[p->unk_150];
        }
    }
    if (idx != -1) {
        func_80005528(p, idx);
        p->unk_144 = val;
        if (idx > 0) {
            func_8001F138(D_800529FC[idx]);
        }
    }
}
