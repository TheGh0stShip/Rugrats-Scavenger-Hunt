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
extern struct Entry_80059DD0 D_80059DD0[];
extern struct Entry_80059DD0 *D_80059ECC;
extern struct Entry_80059DD0 *D_80059ED0;
extern s32 D_800533D8;
extern s32 D_800533DC;
extern void func_80016A50(u8 *, s16);
extern int func_8001F138(s32);

s32 func_80016BDC(s32 buttons) {
    struct Entry_80059DD0 *e = D_80059ED0;
    s32 ret = 0;

    if (e->unk4 == 1) {
        return 1;
    }
    if (e->unk4 != 0) {
        if (e->unk4 == 2) {
            if (e->unkA[1] == 0) {
                ret = D_800533D8 != 0;
            } else {
                e->unk4 = 0;
            }
        }
    } else if (buttons & 0x20) {
        s32 n = D_800533D8 + 1;
        D_800533DC = n;
        D_80059ECC = &D_80059DD0[n];
        D_80059ECC->unkA[0] = 0;
        D_80059ECC->unkE[0] = 0;
        func_80016A50(D_80059ECC->unkA, 0xFF);
        func_80016A50(D_80059ECC->unkE, 0xFF);
        func_80016A50(D_80059ED0->unkA, 0x7F);
        func_8001F138(0x27);
    }
    return ret;
}
