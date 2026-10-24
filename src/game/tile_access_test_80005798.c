/* func_80005798: SN GCC 2.8 -O2 -G0, reviewed scheduling normalization
 * (normalize_d1_func_80005798). The empty asm keeps the mode result in its
 * own register as in retail. */
typedef unsigned short u16;
typedef short s16;
typedef int s32;

extern void *D_800529F0;

extern s16 func_8000DF40(s16 tile);
extern s32 func_80025FC4(void *p, s32 a);
extern s32 func_8001BF00(void);

s32 func_80005798(s16 tile) {
    u16 raw = func_8000DF40(tile);

    tile = raw;
    if (tile == 0x13) {
        return 0;
    }
    if ((u16)(raw - 0xF9) < 4) {
        if (D_800529F0 == 0) {
            return 0;
        }
        if (func_80025FC4(D_800529F0, 15) == 0) {
            return 0;
        }
        { s32 r; __asm__("" : "=r"(r) : "0"(func_8001BF00())); if (r != 0x16) goto one; } return 0;
    }
    return tile != 0;
one:
    return 1;
}
