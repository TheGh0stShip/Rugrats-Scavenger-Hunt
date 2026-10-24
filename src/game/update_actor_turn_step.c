/* func_80021698: per-frame turn/step update for a moving actor. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

struct Actor_80021698 {
    u8 pad_00[0xF8];
    u32 kind;
    u8 pad_FC[0x104 - 0xFC];
    s32 timer;
    u8 pad_108[0x124 - 0x108];
    s32 angle;
    s32 target;
    u8 pad_12C[0x13C - 0x12C];
    u8 tx;
    u8 ty;
    u8 pad_13E[0x144 - 0x13E];
    s32 unk144;
    f32 unk148;
    f32 unk14C;
    u8 pad_150[0x1D4 - 0x150];
    s32 unk1D4;
};

extern s32 D_800535E4;
extern s32 D_80052F18;
extern s32 D_80052F80[4];
extern void func_80009464(struct Actor_80021698 *);
extern s32 func_800219EC(struct Actor_80021698 *);
extern void func_80009610(struct Actor_80021698 *);
extern void *func_800218BC(struct Actor_80021698 *);
extern void func_80005500(s32, struct Actor_80021698 *);
extern s32 func_8000DF14(s32, s32);
extern s32 func_8000DF40(s16);
extern void func_80006E30(struct Actor_80021698 *);
extern f32 func_80006D00(s32, s32, s32);
extern f32 func_80006D7C(s32, s32);

void func_80021698(struct Actor_80021698 *a)
{
    s32 r = 0;
    s32 d;
    s32 t;

    D_800535E4 = 0;
    {
        u32 k = a->kind;
        if (k == 4) goto c4;
        __asm__ __volatile__("" ::: "$2");
        if (k < 4) goto out;
        if (k < 7) goto c56;
        goto out;
c4:
        func_80009464(a);
        if (a->unk1D4 == 1) {
            r = func_800219EC(a);
        }
        if (r == 0) {
            func_80009610(a);
        }
        goto out;
c56:
        func_800218BC(a);
    }
out:
    func_80005500(6, a);
    d = a->angle - a->target;
    if (D_80052F18 == 2) {
        d = 0;
    }
    if (d == 0) {
        func_80005500(1, a);
        if (D_80052F18 != 2) {
            t = func_8000DF40(func_8000DF14(a->ty, a->tx));
            __asm__ __volatile__("" ::: "$4");
            if ((u16)(t - 0xF5) < 4) {
                s32 e = *(D_80052F80 + ((a->target / 90) & 3));
                s16 st;
                __asm__ __volatile__("" ::: "$3");
                st = t;
                if (st == e) {
                    func_80006E30(a);
                }
            }
            return;
        }
        a->unk148 = func_80006D00(0, 1, a->unk144);
        a->unk14C = a->unk148 - func_80006D7C(0, 1);
        return;
    }
    if (0.0f <= (f32)d) {
        if (d == 180) {
            goto turn_around;
        }
        goto check;
    } else if (-d != 180) {
        goto check;
    }
turn_around:
    func_80005500(2, a);
    a->timer = 0x18;
    return;
check:
    if (d == 90 || d == -270) {
        func_80005500(2, a);
    } else {
        func_80005500(3, a);
    }
    a->timer = 0xC;
}
