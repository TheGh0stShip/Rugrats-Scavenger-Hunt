/* func_80017F4C: draw the four-row option menu, preview model and sound cue. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

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

struct Label {
    s32 text;
    s16 x;
    s16 y;
    s32 font;
    u8 pad_0C[4];
};

struct State_80052F18 {
    u32 mode;
    u8 pad_04[8];
    s32 sel[4];
};

extern struct Label D_80053214[];
extern struct Item_80059DB8 D_80059DD0[];
extern struct Item_80059DB8 *D_80059ED0;
extern s32 D_800533D8;
extern struct State_80052F18 D_80052F18;
extern f32 D_80053400;
extern s32 D_80053408;
extern f32 D_800533FC[3];
extern s32 D_80052FEC[];
extern s32 D_80059D48[3];
extern u8 D_80059CF0[];
extern char D_80001580[];
extern char D_80001584[];
extern char D_8000158C[];
extern char D_80001594[];
extern char D_800015A8[];
extern void func_800136D8(u8);
extern void func_800136AC(u8);
extern void func_80013724(void);
extern void func_800137E8(s32);
extern void func_800138FC(void *, s32, s32);
extern void func_8001380C(s32, char *, s32, s32);
extern int func_8003A57C(char *, char *, ...);
extern void func_80016AF8(s32, s32);
extern s32 func_80016AA8(u8 *);
extern void *func_8001E630(void);
extern void func_80047A38(void *, f32, f32, f32, f32, f32, f32, f32);
extern u8 *func_800197C8(s32);
extern void func_8000C530(u8 *, s32, s32);
extern void *func_8000BDFC(void *, s32, s32, s32, void *, void *, int);
extern void func_8000C674(u8 *);

void func_80017F4C(struct Item_80059DB8 *it) {
    char buf[0x20];
    u32 i;
    struct Label *lbl;
    struct Label *lbl2;
    s32 variant;
    s32 sel;
    void *obj;
    s32 snd;
    u8 *p;

    for (i = 0; i < 5; i++) {
        if (i == 0) {
            func_80013724();
        } else if (i - 1 == it->unk8 && it->unk4 == 0) {
            func_800136D8(it->unkE);
        } else if ((s32)i < it->unk8 + 1) {
            func_80013724();
        } else {
            func_800136AC(it->unkA);
        }
        func_800137E8(D_80053214[i].font);
        if (it->unk8 + 1 < (s32)i) {
            func_8003A57C(buf, D_80001584, D_8000158C, i);
            func_800138FC(buf, D_80053214[i].x, D_80053214[i].y);
        } else {
            func_800138FC(D_80053214[i].text, D_80053214[i].x, D_80053214[i].y);
        }
        if ((s32)i > 0) {
            func_800137E8(D_80053214[i + 4].font);
            if (it->unk8 + 1 < (s32)i) {
                func_800138FC(D_80001580, D_80053214[i + 4].x, D_80053214[i + 4].y);
            } else {
                func_800138FC(D_80053214[i + 4].text, D_80053214[i + 4].x, D_80053214[i + 4].y);
            }
        }
    }

    if (it == &D_80059DD0[D_800533D8]) {
if (D_80052F18.mode == 1 && it->unk8 == 0) func_80016AF8(3, 0xFF); else if (D_80059ED0->unk8 >= 3) func_80016AF8(6, 0xFF); else if (D_80052F18.mode == 2) func_80016AF8(3, 0xFF); else func_80016AF8(7, 0xFF);
    }
    it->unk4 = func_80016AA8(&it->unkA);
    func_80016AA8(&it->unkE);

    D_80053400 = -(it->unk8 * 150);
    D_80053408 += 6;

    switch (D_80052F18.mode) {
        case 1:
            variant = 3;
            break;
        case 0:
            variant = 0;
            break;
        case 2:
            variant = 4;
            break;
    }

    sel = D_80052F18.sel[it->unk8];
    obj = func_8001E630();
    func_80047A38(obj, 0.0f, (f32)D_80053408, 0.0f, 1.0f,
                  D_800533FC[0], D_800533FC[1], D_800533FC[2]);
    p = func_800197C8(D_80052FEC[sel] + variant) + 0x2C;
    func_8000C530(p, 1, 0);
    func_8000BDFC(p, D_80059D48[0], D_80059D48[1], D_80059D48[2], D_80059CF0, obj, 0);
    func_8000C674(p);
    func_80013724();
    func_8001380C(0x41, D_80001594, 0x8C, 0xA0);
    func_8001380C(0x42, D_800015A8, 0x8C, 0xB2);
}
