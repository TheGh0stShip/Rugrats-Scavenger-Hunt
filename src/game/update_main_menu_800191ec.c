/* func_800191EC: main menu frame update and per-screen dispatch.
 * Built with SN GCC 2.8 -O2 and reviewed register-rename/reorder normalization */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Item_80059DB8 {
    s32 unk0;
    s32 unk4;
    s16 unk8;
    u8 unkA;
    u8 unkB;
    u8 pad_0C[2];
    u8 unkE;
    u8 pad_0F[9];
};

struct State_80052F18 {
    s32 mode;
    u8 pad_04[2];
    u16 unk6;
    s32 unk8;
    s32 sel[4];
    s32 slot[4];
    u8 pad_2C[8];
};

extern struct Item_80059DB8 D_80059DD0[];
extern struct Item_80059DB8 *D_80059ED0;
extern s32 D_800533D8;
extern s32 D_800533DC;
extern struct State_80052F18 D_80052F18;
extern struct State_80052F18 D_80059ED8;
extern u16 D_80052F1A;
extern u8 D_80052C01;
extern s32 D_80052FD0[];
extern s32 D_80053224;
extern s32 D_80053264;
extern s32 D_8005497C[];
extern u8 D_80059F10[];
extern s32 D_80059F24;
extern u16 D_800530CE;
extern s32 D_800530CC;
extern s32 D_800530D0;
extern s32 D_800B9344;

extern void func_80011560(void);
extern void *func_80038E90(void *, s32, s32);
extern void func_8001F36C(u16);
extern int func_8001F588(s32);
extern void func_800137E8(s32);
extern void func_80013724(void);
extern void func_80016CD4(struct Item_80059DB8 *);
extern void func_80016FE4(struct Item_80059DB8 *);
extern void func_8001749C(struct Item_80059DB8 *);
extern void func_800176FC(struct Item_80059DB8 *);
extern void func_80017BE4(struct Item_80059DB8 *);
extern void func_80017F4C(struct Item_80059DB8 *);
extern void func_8001885C(struct Item_80059DB8 *);
extern void func_80018DDC(struct Item_80059DB8 *);
extern s32 func_8001F1C4(s32);
extern s32 func_80012CE8(void);
extern s32 func_80016530(s32);
extern void func_80012284(void);
extern void func_800122A4(void);
extern void func_80016DB0(s32);
extern void func_80017144(s32);
extern void func_8001757C(s32);
extern void func_800178B8(s32);
extern void func_80017CCC(s32);
extern s32 func_800182E8(s32);
extern void func_80018A34(s32);
extern void func_80018F04(s32);

s32 func_800191EC(void) {
    s32 ret = 0;
    s32 i;
    s32 buttons;

    if (D_800533DC != D_800533D8) {
        D_800533D8 = D_800533DC;
        D_80059ED0 = &D_80059DD0[D_800533DC];
        switch (D_80059ED0->unk0) {
            case 8:
                func_80011560();
                break;
            case 5:
                D_80059ED0->unk8 = D_80052F1A;
                break;
            case 7:
                D_80053224 = D_80052FD0[D_80052F18.sel[0]];
                for (i = 0; i < 4; i++) {
                    if (i < D_80052C01) {
                        D_80052F18.slot[i] = i + 2;
                    } else {
                        D_80052F18.slot[i] = 1;
                    }
                }
                D_80053264 = D_8005497C[D_80052F18.slot[0]];
                func_80038E90(D_80059F10, 0, 0x10);
                break;
            case 4:
                D_80059ED8 = D_80052F18;
                break;
            case 1:
                break;
        }
        D_80059F24 = 0x50;
        func_8001F36C(D_800530CE);
    }
    if (D_80059F24 >= 0) {
        D_80059F24--;
    }
    func_8001F588(0);
    func_800137E8(8);
    func_80013724();

    switch (D_80059ED0->unk0) {
        case 10:
            func_80016CD4(D_80059ED0);
            break;
        case 9:
            func_80016FE4(D_80059ED0);
            break;
        case 1:
            func_8001749C(D_80059ED0);
            break;
        case 5:
            if (D_80059F24 == 0) {
                D_800530CC = (u16)func_8001F1C4(0x11);
            }
            func_800176FC(D_80059ED0);
            break;
        case 6:
            if (D_80059F24 == 0) {
                D_800530CC = (u16)func_8001F1C4(0x12);
            }
            func_80017BE4(D_80059ED0);
            break;
        case 7:
            if (D_80059F24 == 0) {
                D_800530CC = (u16)func_8001F1C4(0x13);
            }
            func_80017F4C(D_80059ED0);
            break;
        case 4:
            if (D_80059F24 == 0) {
                D_800530CC = (u16)func_8001F1C4(0x1C);
            }
            func_8001885C(D_80059ED0);
            break;
        case 8:
            func_80018DDC(D_80059ED0);
            break;
    }

    if (D_800530D0 != 0) {
        D_800530D0--;
        return ret;
    }
    if (func_80012CE8() != 1) {
        return ret;
    }
    i = 0;
    do {
        buttons = func_80016530(i);
        i++;
    } while (i < 4 && buttons == 0);

    if (buttons != 0) {
        D_800530D0 = 4;
        func_80012284();
    } else if (D_800533D8 == 0 && D_800B9344 == 4) {
        func_800122A4();
    }

    switch (D_80059ED0->unk0) {
        case 10:
            func_80016DB0(buttons);
            break;
        case 9:
            func_80017144(buttons);
            break;
        case 1:
            func_8001757C(buttons);
            break;
        case 5:
            func_800178B8(buttons);
            break;
        case 6:
            func_80017CCC(buttons);
            break;
        case 7:
            ret = func_800182E8(buttons);
            break;
        case 4:
            func_80018A34(buttons);
            break;
        case 8:
            func_80018F04(buttons);
            break;
    }
    return ret;
}
