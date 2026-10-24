typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

struct Actor_80021F10 {
    u8 pad_00[0x2C];
    f32 pos[3];
    u8 pad_38[0x13C - 0x38];
    u8 tx;
    u8 ty;
    u8 pad_13E[0x150 - 0x13E];
    s16 room;
    s16 prevRoom;
};

extern s32 D_80057D5C;
extern s32 D_80057D58;
extern struct Actor_80021F10 *func_800055A4(s32);

void func_80021F10(void)
{
    struct Actor_80021F10 *a;
    struct Actor_80021F10 *b;
    s16 room, prevRoom;
    u8 tile[2];
    f32 pos[3];
    s32 i;

    a = func_800055A4(D_80057D5C);
    room = a->room;
    prevRoom = a->prevRoom;
    tile[0] = a->tx;
    tile[1] = a->ty;
    pos[0] = a->pos[0];
    pos[1] = a->pos[1];
    pos[2] = a->pos[2];
    for (i = 0; i < D_80057D58 - 1; i++) {
        a = func_800055A4(D_80057D5C + i);
        b = func_800055A4(D_80057D5C + i + 1);
        a->room = b->room;
        a->prevRoom = b->prevRoom;
        a->tx = b->tx;
        a->ty = b->ty;
        a->pos[0] = b->pos[0];
        a->pos[1] = b->pos[1];
        a->pos[2] = b->pos[2];
    }
    a = func_800055A4(D_80057D5C + i);
    a->room = room;
    a->prevRoom = prevRoom;
    a->tx = tile[0];
    a->ty = tile[1];
    a->pos[0] = pos[0];
    a->pos[1] = pos[1];
    a->pos[2] = pos[2];
}

