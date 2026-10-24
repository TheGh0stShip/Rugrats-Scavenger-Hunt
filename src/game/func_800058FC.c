typedef short s16;
typedef int s32;

extern s16 func_8000DF14(s16 z, s16 x);
extern s32 func_80005798(s16 tile);

s32 func_800058FC(s32 z, s32 x) {
    s32 r;

    if (func_80005798(func_8000DF14(z + 1, x)) != 0) {
        return 0;
    }
    if (func_80005798(func_8000DF14(z, x + 1)) != 0) {
        return 90;
    }
    if (func_80005798(func_8000DF14(z - 1, x)) != 0) {
        return 180;
    }
    r = func_80005798(func_8000DF14(z, x - 1));
    if (r != 0) {
        r = 270;
    }
    return r;
}
