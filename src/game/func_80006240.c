/* Target: func_80006240  VRAM 0x80006240  ROM 0x6E40
 * Size: 0x1C4  Pipeline: SN GCC 2.8 -O2 -G0 (normalized return)
 * Confidence: medium */
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Cam_80052F04 {
    /* 0x000 */ u8 pad_000[0xF0];
    /* 0x0F0 */ s32 unk_F0;
} Cam_80052F04;

typedef struct Actor_80006240 {
    /* 0x000 */ u8 pad_000[0x2C];
    /* 0x02C */ f32 x;
    /* 0x030 */ f32 y;
    /* 0x034 */ f32 z;
    /* 0x038 */ u8 pad_038[0x118 - 0x38];
    /* 0x118 */ u32 flags;
    /* 0x11C */ u8 pad_11C[0x124 - 0x11C];
    /* 0x124 */ s32 unk_124;
    /* 0x128 */ s32 unk_128;
    /* 0x12C */ u8 pad_12C[0x144 - 0x12C];
    /* 0x144 */ s32 unk_144;
    /* 0x148 */ f32 unk_148;
    /* 0x14C */ u8 pad_14C[0x150 - 0x14C];
    /* 0x150 */ s16 unk_150;
} Actor_80006240;

extern Cam_80052F04 *D_80052F04;

extern void func_80012D04(s32 a, s32 b);
extern void func_80006820(Actor_80006240 *p);
extern s32 func_80005598(Actor_80006240 *p);
extern f32 func_80006D00(s16 a, s32 b, s32 c);
extern f32 func_80006DCC(f32 x);
extern void func_8000DDAC(u8 a);
extern void func_80006AE0(f32 *out, s16 a, s32 b, f32 c);
extern void func_80005500(s32 a, Actor_80006240 *p);
extern s16 func_8000E154(s16 a, s32 b);
extern void func_800069C8(s32 a, Actor_80006240 *p, s32 b);

void func_80006240(Actor_80006240 *p) {
    f32 pos[3];
    s32 t;

    func_80012D04(10, 0);
    if (p->flags & 0x400) {
        func_80006820(p);
        D_80052F04->unk_F0 = 0;
    }
    p->unk_148 = func_80006D00(p->unk_150, func_80005598(p) == 0, p->unk_144);
    t = func_80006DCC(p->unk_148);
    p->unk_124 = p->unk_128 = t;
    func_8000DDAC(*((u8 *)p + 0x151));
    func_80006AE0(pos, p->unk_150, func_80005598(p) == 0, p->unk_148);
    p->x = pos[0];
    p->z = pos[2];
    if (p->flags & 0x80) {
        if (func_80005598(p) == 0) {
            func_80005500(0, p);
        } else {
            func_80005500(5, p);
        }
        if (func_8000E154(p->unk_150, p->unk_144) == 1 ||
            func_8000E154(p->unk_150, p->unk_144) == 2 ||
            func_8000E154(p->unk_150, p->unk_144) == 3 ||
            func_8000E154(p->unk_150, p->unk_144) == 4 ||
            func_8000E154(p->unk_150, p->unk_144) == 13) {
            p->flags |= 0x400;
        }
    } else {
        func_80005500(6, p);
    }
    func_800069C8(0, p, 1);
}
