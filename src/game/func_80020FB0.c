typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

struct Actor_80020FB0 {
    u8 pad_00[0x2C];
    f32 x;
    f32 pad_30;
    f32 z;
    u8 pad_38[0xB8];
    f32 unkF0;
    u8 pad_F4[8];
    s32 unkFC;
    u8 pad_100[0x28];
    s32 angle;
    u8 pad_12C[0x10];
    union {
        struct {
            u8 tx;
            u8 ty;
            u8 pad[2];
        } b;
        u32 w;
    } tile;
};

extern struct Actor_80020FB0 *func_80005544(void);
extern void func_80021604(u32, s32 *, s32 *, s32, s32, s32 *);
extern s32 func_80010BD8(s32, s32, void *);

void func_80020FB0(struct Actor_80020FB0 *actor)
{
    struct Actor_80020FB0 *target = func_80005544();
    f32 value = 1.0f;
    s32 found = 0;
    s32 x;
    s32 y;
    s32 buffer[2];

    if (actor == target) {
        found = 1;
    } else if ((actor->tile.w & 0xFFFF0000) == (target->tile.w & 0xFFFF0000)) {
        if (target->unkFC != 1) {
            value = 0.0f;
            found = 1;
        }
    } else if (actor->unkFC == 8) {
        found = 1;
    } else if (target->unkFC == 1) {
        func_80021604(target->angle / 90, &x, &y, target->tile.b.tx,
                      target->tile.b.ty, 0);
        if (x == actor->tile.b.tx && y == actor->tile.b.ty) {
            value = 0.0f;
            found = 1;
        }
    }
    if (found) {
        actor->unkF0 = value;
    } else {
        if (func_80010BD8((s32)actor->x, (s32)actor->z, buffer) != 0) {
            value = 0.0f;
        }
        actor->unkF0 = value;
    }
}
