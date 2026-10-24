typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Entry_80059DD0 {
    s32 unk0;
    s32 unk4;
    s16 unk8;
    u8 unkA[4];
    u8 unkE[4];
    u8 pad_12[0x6];
};
struct Icon_Tbl {
    s32 unk0;
    s16 unk4;
    s16 unk6;
    s32 unk8;
    u8 pad_0C[0x4];
};
struct Tbl_80052F18 {
    s32 unk0;
    u8 pad_04[0x8];
    s32 idx[4];
    s32 arg[4];
    s32 unk2C[2];
};
extern struct Icon_Tbl D_80053124[];
extern struct Tbl_80052F18 D_80052F18;
extern struct Entry_80059DD0 D_80059DD0[];
extern s32 D_800533D8;
extern void func_800136D8(s32);
extern void func_800136AC(s32);
extern void func_80033144(s32, s32, s32, s32);
extern void func_800137E8(s32);
extern void func_800138FC(void *, s32, s32);
extern void func_80013724(void);
extern void *func_80032870(s32);
extern void func_8003295C(void *, s32, s32);
extern void func_80016AF8(s32, s32);
extern s32 func_80016AA8(u8 *);

void func_80016FE4(struct Entry_80059DD0 *arg0) {
    s32 i;
    s32 j;
    struct Tbl_80052F18 *t;
    s32 v;

    for (i = 3, t = &D_80052F18, j = 2; i >= 0; i--) {
        if (j == arg0->unk8) {
            func_800136D8(arg0->unkE[0]);
        } else {
            func_800136AC(arg0->unkA[0]);
        }
        func_80033144(0xFF, 0xFF, 0xFF, arg0->unkA[0]);
        func_800137E8(D_80053124[i].unk8);
        func_800138FC((void *)D_80053124[i].unk0, D_80053124[i].unk4, D_80053124[i].unk6);
        func_80013724();
        switch (j) {
            case 0:
                v = t->unk2C[0];
                break;
            case 1:
                v = t->unk2C[1];
                break;
            default:
                goto next;
        }
        func_8003295C(func_80032870(v + 0x1BA), 0x180, D_80053124[i].unk6);
    next:
        j--;
    }
    if (arg0 == &D_80059DD0[D_800533D8]) {
        func_80016AF8(3, arg0->unkA[0]);
    }
    arg0->unk4 = func_80016AA8(arg0->unkA);
    func_80016AA8(arg0->unkE);
}
