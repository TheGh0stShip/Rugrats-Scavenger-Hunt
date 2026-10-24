/* Target: func_80020874  VRAM 0x80020874  ROM 0x21474
 * Size: 0x2B4  Compiler: SN Systems GCC 2.8 (-O2) */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

struct Room_8000DD88 {
    s16 y0;
    s16 x0;
    s16 h;
    s16 w;
};

struct Level_800581F0 {
    u8 pad_00[4];
    s16 count;
    u8 pad_06[6];
    s16 unkC;
    s16 unkE;
};

struct Actor_80020874 {
    u8 pad_00[0x2C];
    f32 x;
    f32 y;
    f32 z;
    u8 pad_38[0x124 - 0x38];
    s32 angle;
    s32 unk128;
    s32 unk12C;
    u8 pad_130[0x13C - 0x130];
    u8 tx;
    u8 ty;
    u8 pad_13E[0x144 - 0x13E];
    s32 unk144;
    u8 pad_148[0x150 - 0x148];
    s16 room;
    s16 prevRoom;
};

union Coord_80020874 {
    s32 value;
    u8 bytes[4];
    s16 halves[2];
};

struct Locals_80020874 {
    f32 vec[3];
    s32 pad;
    union Coord_80020874 x;
    union Coord_80020874 y;
};

extern s32 D_80052F18;
extern struct Level_800581F0 *D_800581F0;
extern s8 D_8006FB70[];
extern s32 func_8000E078(s32, s32);
extern s32 func_8003A840(void);
extern void func_80005528(struct Actor_80020874 *, s32);
extern void func_80020B28(s32 *, s32 *, s32);
extern struct Room_8000DD88 *func_8000DD88(u8);
extern void func_80021604(u32, s32 *, s32 *, s32, s32, s32 *);
extern s32 func_8000DF14(s32, s32);
extern s32 func_80005798(s16);
extern f32 func_80006D00(s16, s32, s32);
extern void func_80006AE0(f32 *, s16, s32, f32);
extern f32 func_80006DCC(f32);

void func_80020874(struct Actor_80020874 *a)
{
    register struct Actor_80020874 *actor __asm__("$18") = a;
    register s32 any __asm__("$19");
    s32 n;
    s32 i;
    s32 r;
    register s32 v __asm__("$2");
    s32 xv;
    s32 yv;
    f32 f;
    struct Locals_80020874 locals;

    if (D_80052F18 != 2) {
        any = i = 0;
        n = D_800581F0->count;
        D_8006FB70[func_8000E078(D_800581F0->unkC, D_800581F0->unkE)] = 1;
        for (; i < n - 1; i++) {
            if (D_8006FB70[i] == 0) {
                any = 1;
            }
        }
        n--;
        do {
            r = func_8003A840() % n;
        } while (any && D_8006FB70[r] != 0);
        D_8006FB70[r] = 1;
        func_80005528(actor, r);
        actor->prevRoom = actor->room;
        func_80020B28((s32 *)&locals.x.value, (s32 *)&locals.y.value, r);
        actor->tx = locals.x.bytes[3] + func_8000DD88(r)->x0;
        actor->ty = locals.y.bytes[3] + func_8000DD88(r)->y0;
        xv = locals.x.value;
        yv = locals.y.value;
        actor->x = xv * 600 + 300;
        actor->z = yv * 600 + 300;
        {
            register s32 *angle __asm__("$16") = &actor->angle;
            do {
                func_80021604(func_8003A840() % 4,
                              (s32 *)&locals.x.value,
                              (s32 *)&locals.y.value,
                              actor->tx, actor->ty, angle);
            } while (func_80005798(
                         func_8000DF14(locals.y.halves[1],
                                       locals.x.halves[1])) == 0);
        }
        v = actor->angle;
        actor->unk12C = v;
    } else {
        __asm__ volatile ("" : : : "$4");
        func_80005528(actor, 0);
        actor->unk144 = 0;
        actor->prevRoom = actor->room;
        f = func_80006D00(actor->room, 1, 0);
        func_80006AE0((f32 *)locals.vec, actor->room, 1, f);
        actor->x = locals.vec[0];
        actor->y = locals.vec[1];
        actor->z = locals.vec[2];
        v = func_80006DCC(f);
        actor->angle = v;
        actor->unk12C = v;
    }
    actor->unk128 = v;
}
