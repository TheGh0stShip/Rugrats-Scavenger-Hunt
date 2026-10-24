typedef unsigned char u8;
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

struct Label_80053194 {
    s32 text;
    s16 x;
    s16 y;
    s32 font;
    u8 pad_0C[4];
};

extern struct Label_80053194 D_80053194[4];
extern struct Item_80059DB8 D_80059DD0[];
extern s32 D_800533D8;
extern s32 D_80052F20;
extern char D_80001474[];
extern char D_8000157C[];
extern char D_80001580[];
extern void func_8001749C(struct Item_80059DB8 *);
extern void func_800136D8(u8);
extern void func_800136AC(u8);
extern void func_800137E8(s32);
extern void func_800138FC(void *, s32, s32);
extern void func_80033144(s32, s32, s32, s32);
extern void func_8001380C(s32, char *, s32, s32);
extern void func_80016AF8(s32, u8);
extern s32 func_80016AA8(u8 *);

void func_800176FC(struct Item_80059DB8 *item)
{
    u32 i;

    func_8001749C(item - 1);
    for (i = 0; i < 4; i++) {
        if (i - 1 == item->unk8) {
            func_800136D8(item->unkE);
        } else {
            func_800136AC(item->unkA);
        }
        func_800137E8(D_80053194[i].font);
        func_800138FC(D_80053194[i].text, D_80053194[i].x,
                      D_80053194[i].y);
    }
    if (D_800533D8 == 1 && item->unk8 < 2) {
        func_80033144(0xFF, 0xFF, 0xFF, item->unkE);
        func_8001380C(0x42, D_80001474, 0x46, 0xB4);
        if (D_80052F20 != 0) {
            func_800136D8(item->unkE);
        } else {
            func_800136AC(item->unkE);
        }
        func_800138FC(D_8000157C, 0x14F, 0xB4);
        if (D_80052F20 == 0) {
            func_800136D8(item->unkE);
        } else {
            func_800136AC(item->unkE);
        }
        func_800138FC(D_80001580, 0x181, 0xB4);
    }
    if (item == &D_80059DD0[D_800533D8]) {
        func_80016AF8(3, item->unkE);
    }
    item->unk4 = func_80016AA8(&item->unkA);
    func_80016AA8(&item->unkE);
}
