typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

#define ABSF(x) ((x) >= 0.0f ? (x) : -(x))

struct Actor_80022060 {
    u8 pad_00[0x13C];
    u8 tx;
    u8 ty;
};

extern void func_80021604(s16, s32 *, s32 *, s32, s32, s32 *);
extern s32 func_8000DF14(s16, s16);
extern s32 func_80005798(s16);

s16 func_80022060(struct Actor_80022060 *a, u8 *pos)
{
    s16 dx;
    s32 dy;
    s32 first, second;
    s16 dirs[4];
    s32 xy[2];
    s32 i;

    dx = pos[0] - a->tx;
    dy = pos[1] - a->ty;
    if (dy > 0) {
        if (ABSF(dx) < dy) {
            first = 0;
            if (dx < 0) second = 270; else second = 90;
        } else if (dx > 0) {
            first = 90;
            second = 0;
        } else {
            first = 270;
            second = 0;
        }
    } else {
        if (ABSF(dx) < -dy) {
            first = 180;
            if (dx < 0) second = 270; else second = 90;
        } else {
            if (dx > 0) { first = 90; second = 180; } else { first = 270; second = 180; }
        }
    }
    dirs[1] = second;
    dirs[0] = first;
    dirs[2] = (second + 180) % 360;
    dirs[3] = (first + 180) % 360;
    for (i = 0; i < 4; i++) {
        func_80021604(dirs[i] / 90, &xy[0], &xy[1], a->tx, a->ty, 0);
        if (func_80005798(func_8000DF14(xy[1], xy[0])) != 0) {
            break;
        }
    }
    return i[dirs];
}
