typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;

typedef struct {
    s32 kind;
    s32 state;
    s16 page;
    u8 pad0A;
    u8 finished;
    s16 fade_a;
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
    MenuState *menu = D_80059ED0;
    s32 result = 0;
    s32 horizontal = 0;
    s32 vertical = 0;
    s32 i;
    s32 value;

    switch (menu->state) {
    case 0:
        break;
    case 2:
        if (menu->finished != 0) {
            menu->state = 0;
        } else {
            D_800533DC = D_800533D8 - 1;
            D_80059ECC = &D_80059DD0[D_800533DC];
        }
        return 0;
    default:
        return 0;
    }

    if (buttons & 0x20) {
        if (D_80052F18.mode == 2) {
            if (menu->page < 3) {
                return 0;
            }
        } else if (D_80052F18.mode != 0 && menu->page == 0) {
            return 0;
        }

        menu->page++;
        D_80059F10[D_80052F18.primary[menu->page]] = 1;
        for (i = menu->page; i < 4; i++) {
            D_80052F18.secondary[i] = 0;
        }
        return 1;
    }

    if (buttons & 0x10) {
        if (menu->page >= 3) {
            return 0;
        }
        D_80059F10[D_80052F18.primary[menu->page]] = 1;
        menu->page++;
        if (menu->page >= 5) {
            result = 1;
        } else if (D_80059F10[D_80052F18.primary[menu->page]] != 0) {
            for (i = 0; i < 4; i++) {
                if (D_80059F10[i] == 0) {
                    D_80052F18.primary[menu->page] = i;
                    break;
                }
            }
        }
        D_80053214[menu->page + 1].value =
            D_80052FD0[D_80052F18.primary[menu->page]];
        D_80053214[menu->page + 5].value =
            D_8005497C[D_80052F18.secondary[menu->page]];
        func_8001F138(0x27);
        return result;
    }

    if (buttons & 0x40) {
        if (menu->page != 0) {
            menu->page--;
            D_80059F10[D_80052F18.primary[menu->page]] = 0;
        } else {
            func_80016A50(&menu->fade_a, 0);
            func_80016A50(&menu->fade_b, 0);
            for (i = 0; i < D_800533D8; i++) {
                func_80016A50(&D_80059DD0[i].fade_a,
                              i == D_800533D8 - 1 ? 0xFF : 0x7F);
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

    if (horizontal == 0 && vertical == 0) {
        return 0;
    }
    func_8001F138(0x25);

    do {
        value = D_80052F18.primary[menu->page] + horizontal + 4;
        if (value < 0) {
            value += 3;
        }
        value -= (value / 4) * 4;
        D_80052F18.primary[menu->page] = value;
    } while (D_80059F10[value] == 0);

    D_80053214[menu->page + 1].value = D_80052FD0[value];

    do {
        value = D_80052F18.secondary[menu->page] + D_80059F20 + vertical;
        value %= D_80059F20;
        D_80052F18.secondary[menu->page] = value;
    } while (value == 0);

    D_80053214[menu->page + 5].value = D_8005497C[value];
    return 0;
}
