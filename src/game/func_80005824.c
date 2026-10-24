typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct Actor_80005824 {
    u8 pad_000[0x124];
    s32 angle;
} Actor_80005824;

extern s16 func_8000DF14(s16 z, s16 x);
extern s32 func_80005798(s16 tile);

s32 func_80005824(Actor_80005824 *p, s32 z, s32 x) {
    switch (p->angle) {
    case 0:
        if (func_80005798(func_8000DF14(z - 1, x))) {
            return 1;
        }
        break;
    case 90:
        if (func_80005798(func_8000DF14(z, x - 1))) {
            return 1;
        }
        break;
    case 180:
        if (func_80005798(func_8000DF14(z + 1, x))) {
            return 1;
        }
        break;
    case 270:
        if (func_80005798(func_8000DF14(z, x + 1))) {
            return 1;
        }
        break;
    }
    return 0;
}
