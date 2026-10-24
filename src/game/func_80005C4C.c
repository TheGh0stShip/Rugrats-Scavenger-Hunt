typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Actor_80005C4C {
    u8 pad_000[0xE0];
    s32 unk_E0;
    u8 pad_0E4[0x118 - 0xE4];
    u32 flags;
    u8 pad_11C[0x124 - 0x11C];
    s32 unk_124;
    s32 unk_128;
    u8 pad_12C[0x148 - 0x12C];
    f32 unk_148;
    u8 pad_14C[0x150 - 0x14C];
    s16 unk_150;
} Actor_80005C4C;

extern s32 D_80052F18;
extern s32 D_80057D58;
extern s32 D_80057D5C;
extern s32 D_800581E0;

extern int func_80010CC0(s32 i);
extern void func_80010FB4(s32 i);
extern Actor_80005C4C *func_80020680(void);
extern Actor_80005C4C *func_800055A4(s32 i);
extern s32 func_80019740(s32 a);
extern void func_800069C8(s32 a, Actor_80005C4C *p, s32 b);
extern void func_80005500(s32 a, Actor_80005C4C *p);
extern s32 func_8000E108(void);
extern f32 func_80006DCC(f32 x);
extern void func_8000DFD8(void);
extern s32 func_8001BF3C(void);
extern void func_8001BF24(s32 a);

void func_80005C4C(void) {
    Actor_80005C4C *p;
    s32 i;
    s32 v;
    s32 t;

    for (v = 0; v < 4; v++) {
        func_80010CC0(v);
        func_80010FB4(v);
    }
    if (D_80052F18 != 2) {
        p = func_80020680();
        if (p->flags & 0x100) {
            p->flags &= ~0x100;
        }
    }
    for (i = 0; i < D_80057D5C + D_80057D58; i++) {
        p = func_800055A4(i);
        if (!(i < D_80057D5C) || func_80019740(p->unk_E0) != 0) {
            func_800069C8(0, p, 1);
        }
        p->flags &= ~0x400;
        p->flags &= ~0x400000;
        p->flags &= ~0x800000;
        p->flags &= ~0x1000000;
        func_80005500(0, p);
        v = p->unk_150;
        if (v != func_8000E108()) {
            p->flags &= ~0x100000;
        }
        if (D_80052F18 == 2 && i < D_80057D5C) {
            t = func_80006DCC(p->unk_148);
            p->unk_124 = p->unk_128 = t;
        }
    }
    if (D_800581E0 == 4) {
        func_8000DFD8();
    }
    if (func_8001BF3C() == 0x12) {
        func_8001BF24(0);
    }
}
