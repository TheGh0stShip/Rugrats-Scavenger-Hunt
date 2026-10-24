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
    u8 pad_00[6];
    s16 exitY;
    s16 exitX;
};

struct Actor_80020B28 {
    u8 pad_00[0x13C];
    u8 tx;
    u8 ty;
};

extern s32 D_80057D5C;
extern s32 D_80057D58;
extern struct Level_800581F0 *D_800581F0;
extern struct Room_8000DD88 *func_8000DD88(u8);
extern s32 func_8003A840(void);
extern struct Actor_80020B28 *func_800055A4(s32);
extern s32 func_8000DE80(u8, s16, s16);
extern s32 func_8000DF40(s16);

void func_80020B28(s32 *px, s32 *py, s32 room)
{
    struct Room_8000DD88 *r = func_8000DD88(room);
    s32 i;
    s32 occupied;
    s16 t;

    do {
        i = 0;
        *px = func_8003A840() % r->w;
        *py = func_8003A840() % r->h;
        occupied = 0;
        for (; i < D_80057D5C + D_80057D58; i++) {
            if (*px + r->x0 == func_800055A4(i)->tx &&
                *py + r->y0 == func_800055A4(i)->ty) {
                occupied = 1;
                break;
            }
        }
        t = func_8000DF40(func_8000DE80(room, *py, *px));
    } while (t == 0 || t == 0xF7 || t == 0xF8 || t == 0xF6 || t == 0xF5 || occupied ||
             (*py + r->y0 == D_800581F0->exitY && *px + r->x0 == D_800581F0->exitX) ||
             (u16)(t - 0xF9) < 4);
}

