/* Target: func_80005E3C  VRAM 0x80005E3C  ROM 0x6A3C
 * Size: 0xBC  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

typedef struct Actor_80005E3C {
    /* 0x000 */ u8 pad_000[0x118];
    /* 0x118 */ u32 flags;
} Actor_80005E3C;

extern s32 D_80057D58;
extern s32 D_80057D5C;

extern Actor_80005E3C *func_800055A4(s32 i);

void func_80005E3C(Actor_80005E3C *a, Actor_80005E3C *b) {
    u32 i;
    Actor_80005E3C *p;

    for (i = 0; i < (u32)(D_80057D5C + D_80057D58); i++) {
        p = func_800055A4(i);
        if (p != a && p != b) {
            p->flags |= 0x100000;
        }
    }
}
