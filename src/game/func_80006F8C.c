/* Target: func_80006F8C  VRAM 0x80006F8C  ROM 0x7B8C
 * Size: 0x190  Compiler: SN GCC 2.8 -O2 -G0 (normalized return)
 * Confidence: high
 * Pipeline: SN GCC 2.8 -O2 -G0 (normalized return) */
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

typedef struct Actor_80006F8C {
    /* 0x000 */ u8 pad_000[0x118];
    /* 0x118 */ u32 flags;
    /* 0x11C */ u8 pad_11C[0x130 - 0x11C];
    /* 0x130 */ s32 unk_130;
    /* 0x134 */ u8 pad_134[0x13E - 0x134];
    /* 0x13E */ u8 lastX;
    /* 0x13F */ u8 lastZ;
} Actor_80006F8C;

typedef struct Level_800581F0 {
    /* 0x0000 */ u8 pad_0000[0x2014];
    /* 0x2014 */ s16 originZ;
    /* 0x2016 */ s16 originX;
} Level_800581F0;

extern u8 *D_800581F0;
extern s8 D_800581E9;
extern s32 D_800529F8;
extern s16 D_80057DA0;
extern u8 D_80057D68[][2];

extern s32 func_80005798(s16 tile);
extern s32 func_80025FC4(Actor_80006F8C *p, s32 a);

s32 func_80006F8C(Actor_80006F8C *p, s32 x, s32 z, s32 *outX, s32 *outZ) {
    u8 *e;
    Level_800581F0 *lvl;

    e = (u8 *)(z * 0x80 + (u32)D_800581F0);
    e += x * 2;
    if (func_80005798(*(s16 *)(e + 0x12)) == 0) {
        return 0;
    }
    lvl = (Level_800581F0 *)(D_800581F0 + D_800581E9 * 0x210);
    *outX = (x - lvl->originX) * 600 + 300;
    lvl = (Level_800581F0 *)(D_800581F0 + D_800581E9 * 0x210);
    *outZ = (z - lvl->originZ) * 600 + 300;
    if (D_800529F8 == p->unk_130 && !(p->flags & 0x20) && func_80025FC4(p, 14) == 0) {
        if (x == p->lastX && z == p->lastZ) {
            return 0;
        }
    }
    if (D_800529F8 != 0) {
        return 1;
    }
    e = (u8 *)D_80057D68;
    e += (D_80057DA0 - 1) << 1;
    if (e[0] != x || e[1] != z) {
        return 0;
    }
    return 1;
}
