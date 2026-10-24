/* func_80006E88: KMC GCC 2.7.2 -O2 -G0 -mips3, reviewed scheduling
 * normalization (normalize_d1_func_80006E88). The empty asm makes the two
 * zero arguments share one register as in retail. */
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

typedef struct Actor_80006E88 {
    /* 0x000 */ u8 pad_000[0xFC];
    /* 0x0FC */ s32 state;
    /* 0x100 */ u8 pad_100[4];
    /* 0x104 */ s32 timer;
    /* 0x108 */ u8 pad_108[0x124 - 0x108];
    /* 0x124 */ s32 angle;
    /* 0x128 */ s32 targetAngle;
} Actor_80006E88;

extern void func_80005500(s32 a, Actor_80006E88 *p);
extern void func_800069C8(s32 anim, Actor_80006E88 *p, s32 loop);

s32 func_80006E88(Actor_80006E88 *p) {
    s32 n;

    if ((u32)(p->state - 2) >= 2) {
        return 0;
    }
    n = --p->timer;
    if (n <= 0) {
        p->angle = p->targetAngle;
        func_80005500(6, p);
        { s32 z; __asm__("" : "=r"(z) : "0"(0)); func_800069C8(z, p, z); }
        p->timer = 0;
    } else if (p->state == 2) {
        p->angle = (p->angle - (u8)((n & 1) + 7) + 360) % 360;
        func_800069C8(4, p, 0);
    } else {
        p->angle = (p->angle + (u8)((n & 1) + 7)) % 360;
        func_800069C8(3, p, 0);
    }
    return 1;
}
