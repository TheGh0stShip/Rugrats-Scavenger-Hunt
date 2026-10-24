/* Target: func_8001C404  VRAM 0x8001C404  ROM 0x1D004
 * Size: 0x15C  Compiler: SN GCC 2.8 (-O2), reviewed scheduling normalization */
typedef unsigned char u8;
typedef signed char s8;
typedef signed int s32;

struct Player_800055A4 {
    u8 pad_00[0x1A7];
    s8 unk1A7;   /* 0x1A7 */
};

struct Task_80053634 {
    u8 pad_00[0x28];
    s32 mode;    /* 0x28 */
};

struct Spawned_800112D0 {
    u8 pad_00[0xE0];
    void *owner; /* 0xE0 */
};

extern s32 D_80057D40;
extern struct Task_80053634 *D_80053634;
extern s32 D_80053848[][6];
extern struct Player_800055A4 *func_800055A4(s32);
extern void *func_8001C560(void);
extern struct Spawned_800112D0 *func_800112D0(s32, s32, s32, s32, s32, s32);

void func_8001C404(void) {
    struct Player_800055A4 *pl = func_800055A4(D_80057D40);
    s32 mode;
    s32 count;
    s32 i;
    s32 *p;

    switch (D_80053634->mode) {
        case 17:
            mode = 0;
            count = 2;
            break;
        case 18:
            mode = 0;
            count = 4;
            break;
        case 20:
            mode = 1;
            count = 2;
            break;
        case 19:
            mode = 1;
            count = 1;
            break;
        case 11:
            mode = 1;
            count = 6;
            break;
    }
    if (mode == 0) {
        if (pl->unk1A7 + count >= 99) {
            count = 99 - pl->unk1A7;
        }
    } else {
        if (pl->unk1A7 - count < 0) {
            count = pl->unk1A7;
        }
    }
    pl = func_8001C560();
    for (i = 0; i < count; i++) {
        p = D_80053848[count];
        func_800112D0(0x1AB, p[i], -360, -4000, mode, i * 5)->owner = pl;
    }
}
