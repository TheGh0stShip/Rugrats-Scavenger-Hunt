typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef float f32;

typedef struct Actor_80006534 {
    u8 pad_000[0x2C];
    f32 x;
    f32 y;
    f32 z;
    u8 pad_038[0x124 - 0x38];
    s32 unk_124;
    s32 unk_128;
    u8 pad_12C[0x144 - 0x12C];
    s32 unk_144;
    f32 unk_148;
    u8 pad_14C[0x150 - 0x14C];
    s16 unk_150;
} Actor_80006534;

extern void func_80005528(Actor_80006534 *p, s32 v);
extern s32 func_80005598(Actor_80006534 *p);
extern f32 func_80006D00(s16 a, s32 b, s32 c);
extern f32 func_80006DCC(f32 x);
extern void func_80006AE0(f32 *out, s16 a, s32 b, f32 c);

void func_80006534(Actor_80006534 *p, s32 area, s32 pos) {
    f32 v[3];
    s32 t;

    func_80005528(p, area);
    p->unk_144 = pos;
    p->unk_148 = func_80006D00(p->unk_150, func_80005598(p) == 0, p->unk_144);
    t = func_80006DCC(p->unk_148);
    p->unk_124 = p->unk_128 = t;
    func_80006AE0(v, p->unk_150, func_80005598(p) == 0, p->unk_148);
    p->x = v[0];
    p->z = v[2];
}
