typedef unsigned char u8;
typedef short s16;
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

struct Label {
    s32 text;
    s16 x;
    s16 y;
    s32 font;
    u8 pad_0C[4];
};

struct State_80052F18 {
    s32 mode;
    u8 pad_04[4];
    s32 unk8;
    u8 pad_0C[0x20];
    s32 unk2C;
    s32 unk30;
};

extern struct Label D_800532A4[4];
extern struct State_80052F18 D_80052F18;
extern s32 D_80054964[];
extern struct Item_80059DB8 D_80059DD0[];
extern s32 D_800533D8;
extern void func_8001749C(struct Item_80059DB8 *);
extern void func_800136D8(u8);
extern void func_800136AC(u8);
extern void func_800137E8(s32);
extern void func_800138FC(void *, s32, s32);
extern void func_80033144(s32, s32, s32, s32);
extern void *func_80032870(s32);
extern void func_8003295C(void *, s32, s32);
extern void func_800139F8(void *, s32, s32);
extern void func_80013724(void);
extern void func_80016AF8(s32, u8);
extern s32 func_80016AA8(u8 *);

void func_8001885C(struct Item_80059DB8 *it) {
    s32 i;
    register struct Label *labels __asm__("$19");
    register struct Label *label __asm__("$17");
    register struct Label *label2 __asm__("$21");
    register s32 *values __asm__("$22");
    register struct State_80052F18 *state __asm__("$20");

    func_8001749C(it - 1);
    i = 3;
    state = &D_80052F18;
    values = D_80054964;
    labels = D_800532A4;
    label2 = &labels[2];
    label = &labels[3];
    for (; i >= 0; i--) {
        if (i == it->unk8) {
            func_800136D8(it->unkE);
        } else {
            func_800136AC(it->unkA);
        }
        func_800137E8(label->font);
        func_800138FC(label->text, label->x, label->y);
        func_80033144(0xFF, 0xFF, 0xFF, it->unkE);
        switch (i) {
            case 0:
                func_8003295C(func_80032870(state->unk2C + 0x1BA),
                              0x180, labels[0].y);
                break;
            case 1:
                func_8003295C(func_80032870(state->unk30 + 0x1BA),
                              0x180, labels[1].y);
                break;
            case 2:
                func_800139F8(values[state->unk8], 0x180, label2->y);
                break;
        }
        func_80013724();
        label--;
    }
    func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
    if (it == &D_80059DD0[D_800533D8]) {
        func_80016AF8(2, it->unkA);
    }
    it->unk4 = func_80016AA8(&it->unkA);
    func_80016AA8(&it->unkE);
}
