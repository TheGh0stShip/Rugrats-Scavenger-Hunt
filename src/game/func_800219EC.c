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
};

struct Actor_800219EC {
    u8 pad_00[0x128];
    s32 target;
    u8 pad_12C[0x13C - 0x12C];
    u8 tx;
    u8 ty;
    u8 pad_13E[0x150 - 0x13E];
    s16 room;
};

extern struct Level_800581F0 *D_800581F0;
extern s32 func_8000E008(s16);
extern int func_80009194(short, int *);
extern struct Room_8000DD88 *func_8000DD88(u8);
extern s32 func_8000DE80(u8, s16, s16);
extern s32 func_8000DF40(s16);
extern s32 func_80021C3C(struct Actor_800219EC *, u8 *);
extern float sqrtf(float);

s32 func_800219EC(struct Actor_800219EC *a)
{
    s16 bestRoom = -1;
    s32 bestVal = -1;
    s16 i = 0;
    s32 found = 0;
    f32 best = 2147483648.0f;
    s32 v;
    struct Room_8000DD88 *r;
    s32 x, y;
    f32 dx, dy, d;
    u8 pos[2];

    for (; i < D_800581F0->count - 1; i++) {
        if (func_8000E008(i) != 0) {
            func_80009194(i, &v);
            if (bestVal < v || (v == bestVal && i == a->room)) {
                bestVal = v;
                bestRoom = i;
            }
        }
    }
    if (bestRoom != -1) {
        r = func_8000DD88(bestRoom);
        for (y = 0; y < r->h; y++) {
            for (x = 0; x < r->w; x++) {
                if ((s16)func_8000DF40(func_8000DE80(bestRoom, y, x)) == 8 &&
                    !(a->tx == x && a->ty == y)) {
                    dx = a->tx - r->x0 + x;
                    dy = a->ty - r->y0 + y;
                    d = sqrtf(dx * dx + dy * dy);
                    if (d < best) {
                        best = d;
                        pos[0] = r->x0 + x;
                        found = 1;
                        pos[1] = r->y0 + y;
                    }
                }
            }
        }
    }
    if (found) {
        a->target = func_80021C3C(a, pos);
    }
    return found;
}

