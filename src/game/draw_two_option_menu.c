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
extern struct Icon_Tbl D_80053104[];
extern void func_80013704(void);
extern void func_800136D8(s32);
extern void func_800136AC(s32);
extern void func_800137E8(s32);
extern void func_800138FC(void *, s32, s32);
extern void func_80016AF8(s32, s32);
extern s32 func_80016AA8(u8 *);

void func_80016CD4(struct Entry_80059DD0 *arg0) {
    s32 i;

    for (i = 1; i >= 0; i--) {
        if (i == 2) {
            func_80013704();
        } else if (i - 1 == arg0->unk8) {
            func_800136D8(arg0->unkE[0]);
        } else {
            func_800136AC(arg0->unkA[0]);
        }
        func_800137E8(D_80053104[i].unk8);
        func_800138FC((void *)D_80053104[i].unk0, D_80053104[i].unk4, D_80053104[i].unk6);
    }
    func_80016AF8(3, arg0->unkA[0]);
    arg0->unk4 = func_80016AA8(arg0->unkA);
    func_80016AA8(arg0->unkE);
}
