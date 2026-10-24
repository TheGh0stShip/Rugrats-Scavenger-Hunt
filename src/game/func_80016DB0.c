typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

struct Entry_80016DB0 {
    s32 unk0;
    s32 unk4;
    s16 unk8;
    u8 unkA[4];
    u8 unkE[4];
    u8 pad_12[2];
    s32 unk14;
};

extern struct Entry_80016DB0 D_80059DD0[];
extern struct Entry_80016DB0 *D_80059ECC;
extern struct Entry_80016DB0 *D_80059ED0;
extern s32 D_800533D8;
extern s32 D_800533DC;
extern s32 D_80059D40;
extern void func_80016A50(u8 *, s16);

void func_80016DB0(s32 buttons) {
    struct Entry_80016DB0 *entry = D_80059ED0;
    s32 i = 0;

    if (entry->unk4 != 0) {
        if (entry->unk4 == 2) {
            if (entry->unkA[1] == 0) {
                if (entry->unk14 == 0x2A) {
                    D_80059D40 = 7;
                } else if (D_800533D8 != 0) {
                    s32 n = D_800533D8 - 1;
                    D_800533DC = n;
                    D_80059ECC = &D_80059DD0[n];
                }
            } else {
                entry->unk4 = 0;
            }
        }
    } else {
        if (buttons & 0x10) {
            entry->unk14 = 0x2A;
            func_80016A50(entry->unkA, 0);
            func_80016A50(D_80059ED0->unkE, 0);
        } else if (buttons & 0x40) {
            func_80016A50(entry->unkA, 0);
            func_80016A50(D_80059ED0->unkE, 0);
            for (; i < D_800533D8; i++) {
                if (i == D_800533D8 - 1) {
                    func_80016A50(D_80059DD0[i].unkA, 0xFF);
                } else {
                    func_80016A50(D_80059DD0[i].unkA, 0x7F);
                }
                func_80016A50(D_80059DD0[i].unkE, 0xFF);
                D_80059DD0[i].unk14 = -1;
            }
        }
        D_80059ED0->unk8 = 0;
    }
}
