typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;

typedef struct {
    s32 kind;
    s32 state;
    s16 page;
    union {
        s16 fade_a;
        struct {
            u8 pad0A;
            u8 finished;
        } bytes;
    } fade;
    u8 pad0C[2];
    s16 fade_b;
} MenuState;

typedef struct {
    u8 pad00[10];
    s16 fade_a;
    u8 pad0C[2];
    s16 fade_b;
    u8 pad10[8];
} MenuEntry;

typedef struct {
    s32 mode;
    s32 pad04[2];
    s32 primary[4];
    s32 secondary[4];
} MenuOptions;

typedef struct {
    s32 value;
    u8 pad04[12];
} DisplaySlot;

extern MenuState *D_80059ED0;
extern MenuEntry *D_80059ECC;
extern MenuEntry D_80059DD0[];
extern s32 D_800533D8;
extern s32 D_800533DC;
extern MenuOptions D_80052F18;
extern s32 D_80059F10[4];
extern DisplaySlot D_80053214[];
extern s32 D_80052FD0[];
extern s32 D_8005497C[];
extern s32 D_80059F20;

extern void func_80016A50(void *, s32);
extern void func_8001F138(s32);

s32 func_800182E8(s32 buttons)
{
    s32 result = 0;
    s32 horizontal = 0;
    s32 vertical;
    s32 i;
    s32 value;

    vertical = horizontal;

    if (D_80059ED0->state == 1) {
        return 0;
    }
    if (D_80059ED0->state != 0) {
        if (D_80059ED0->state != 2) {
            return result;
        }
        if (D_80059ED0->fade.bytes.finished != 0) {
            D_80059ED0->state = 0;
        } else {
            D_800533DC = D_800533D8 - 1;
            D_80059ECC = &D_80059DD0[D_800533DC];
        }
        return result;
    }

    if (buttons & 0x20) {
        if (D_80052F18.mode != 0) {
            if (D_80052F18.mode == 2) {
                if (D_80059ED0->page < 3) {
                    return 0;
                }
            } else {
                if (D_80059ED0->page == 0) {
                    return 0;
                }
            }
        }

        D_80059ED0->page++;
        D_80059F10[D_80052F18.primary[D_80059ED0->page]] = 1;
        for (i = D_80059ED0->page; i < 4; i++) {
            D_80052F18.secondary[i] = 0;
        }
        return 1;
    }

    if (buttons & 0x10) {
        if (D_80059ED0->page >= 3) {
            return 0;
        }
        D_80059F10[D_80052F18.primary[D_80059ED0->page]] = 1;
        D_80059ED0->page++;
        if (D_80059ED0->page >= 5) {
            result = 1;
        } else {
            if (D_80059F10[D_80052F18.primary[D_80059ED0->page]] != 0) {
                for (i = 0; i < 4; i++) {
                    if (D_80059F10[i] == 0) {
                        D_80052F18.primary[D_80059ED0->page] = i;
                        break;
                    }
                }
            }
            D_80053214[D_80059ED0->page + 1].value =
                D_80052FD0[D_80052F18.primary[D_80059ED0->page]];
            D_80053214[D_80059ED0->page + 5].value =
                D_8005497C[D_80052F18.secondary[D_80059ED0->page]];
        }
        func_8001F138(0x27);
        return result;
    }

    if (buttons & 0x40) {
        if (D_80059ED0->page != 0) {
            D_80059ED0->page--;
            D_80059F10[D_80052F18.primary[D_80059ED0->page]] = 0;
        } else {
            func_80016A50(&D_80059ED0->fade.fade_a, 0);
            func_80016A50(&D_80059ED0->fade_b, 0);
            for (i = 0; i < D_800533D8; i++) {
                if (i == D_800533D8 - 1) {
                    value = 0xFF;
                } else {
                    value = 0x7F;
                }
                func_80016A50(&D_80059DD0[i].fade_a, value);
                func_80016A50(&D_80059DD0[i].fade_b, 0xFF);
            }
        }
        func_8001F138(0x26);
        return 0;
    }

    if (buttons & 4) {
        horizontal = -1;
    } else if (buttons & 8) {
        horizontal = 1;
    } else if (buttons & 2) {
        vertical = 1;
    } else if (buttons & 1) {
        vertical = -1;
    }

    if (horizontal != 0) {
        func_8001F138(0x25);
    } else if (vertical != 0) {
        func_8001F138(0x25);
    }

    do {
        value = D_80052F18.primary[D_80059ED0->page] + (horizontal + 4);
        value %= 4;
        D_80052F18.primary[D_80059ED0->page] = value;
    } while (D_80059F10[D_80052F18.primary[D_80059ED0->page]] != 0);

    D_80053214[D_80059ED0->page + 1].value =
        D_80052FD0[D_80052F18.primary[D_80059ED0->page]];

    do {
        value = D_80052F18.secondary[D_80059ED0->page] + D_80059F20 + vertical;
        value %= D_80059F20;
        D_80052F18.secondary[D_80059ED0->page] = value;
    } while (D_80052F18.secondary[D_80059ED0->page] == 0);

    D_80053214[D_80059ED0->page + 5].value =
        D_8005497C[D_80052F18.secondary[D_80059ED0->page]];
    return 0;
}
