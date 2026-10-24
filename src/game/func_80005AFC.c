typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

typedef struct Actor_80005AFC {
    u8 pad_000[0xE0];
    s32 unk_E0;
    u8 pad_0E4[0xF4 - 0xE4];
    s32 unk_F4;
    u8 pad_0F8[0x11C - 0xF8];
    s32 unk_11C;
    u8 pad_120[0x124 - 0x120];
    s32 unk_124;
    s32 unk_128;
    s32 unk_12C;
    u8 pad_130[0x13C - 0x130];
    u8 gridX;
    u8 gridZ;
    u8 pad_13E[0x151 - 0x13E];
    u8 unk_151;
} Actor_80005AFC;

typedef struct Unk_800124A0 {
    u8 pad_0[0xC];
    u32 flags;
} Unk_800124A0;

extern s32 D_80052F18;
extern Actor_80005AFC *D_800529F0;
extern s32 D_800529F4;
extern s32 D_800529FC;
extern s32 D_80057D40;
extern s32 D_80057D58;
extern s32 D_80057D5C;
extern u8 *D_800581F0;

extern void func_80005C4C(void);
extern Unk_800124A0 *func_800124A0(void);
extern s32 func_800059EC(void);
extern Actor_80005AFC *func_800055A4(s32 i);
extern void func_80012D04(s32 a, s32 b);
extern void func_8000DDAC(u8 a);
extern void *func_800197C8(s32 a);
extern s32 func_8000E078(s32 z, s32 x);
extern void func_80006404(Actor_80005AFC *p);

void func_80005AFC(Actor_80005AFC *p) {
    Actor_80005AFC *next;
    Actor_80005AFC *other;
    s32 n;
    s32 t;

    func_80005C4C();
    if (func_800124A0() != 0) {
        func_800124A0()->flags |= 1;
    }
    if (func_800059EC() != 0) {
        return;
    }
    t = p->unk_F4;
    n = t + 1;
    if (t == 0) {
        D_800529FC++;
    }
    if (!(n < D_80057D5C + D_80057D58)) {
        n = 0;
    }
    next = func_800055A4(n);
    n = next->unk_F4;
    D_80057D40 = n;
    other = func_800055A4(n);
    D_800529F0 = other;
    D_800529F4 = other->unk_11C;
    func_80012D04(0, 0);
    func_8000DDAC(next->unk_151);
    func_800197C8(next->unk_E0);
    if (D_80052F18 != 2) {
        if (func_8000E078(p->gridZ, p->gridX) == *(s16 *)(D_800581F0 + 4) - 1) {
            p->unk_124 = 0;
            p->unk_128 = 0;
        } else {
            t = p->unk_12C;
            p->unk_124 = t;
            p->unk_128 = t;
        }
    }
    func_80006404(next);
}
