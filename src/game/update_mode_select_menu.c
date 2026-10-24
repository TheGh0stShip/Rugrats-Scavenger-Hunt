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

extern struct Item_80059DB8 D_80059DD0[];
extern struct Item_80059DB8 D_80059DE8[];
extern struct Item_80059DB8 *D_80059ECC;
extern struct Item_80059DB8 *D_80059ED0;
extern s32 D_800533D8;
extern s32 D_800533DC;
extern s32 D_80052F1C;
extern void func_80016A50(u8 *, s32);
extern int func_8001F138(s32);

void func_80017CCC(s32 buttons) {
    struct Item_80059DB8 *it = D_80059ED0;
    s32 delta = 0;
    s32 i;

    if (it->unk4 == 1) {
        return;
    }
    if (it->unk4 != 0) {
        if (it->unk4 == 2) {
            if (it->unkB != 0) {
                it->unk4 = 0;
            } else if (D_80059ECC->unk0 == 7) {
                s32 n = D_800533D8 + 1;
                D_800533DC = n;
                D_80059ECC = &D_80059DD0[n];
                D_80059ECC->unkA = 0;
                D_80059ECC->unkE = 0;
                func_80016A50(&D_80059ECC->unkA, 0x7F);
                func_80016A50(&D_80059ECC->unkE, 0xFF);
                D_80059ECC->unk8 = 0;
            } else {
                func_80016A50(&it->unkA, 0);
                func_80016A50(&D_80059ED0->unkE, 0);
                if (D_800533D8 != 0) {
                    D_800533DC = D_800533D8 - 1;
                }
            }
        }
        return;
    }
    if (buttons & 0x10) {
        D_80052F1C = it->unk8;
        D_80059ECC = &D_80059DD0[D_800533D8 + 1];
        D_80059ECC->unk0 = 7;
        for (i = 0; i <= D_800533D8; i++) {
            func_80016A50(&D_80059DD0[i].unkA, 0);
            func_80016A50(&D_80059DD0[i].unkE, 0);
        }
        func_8001F138(0x27);
    } else if (buttons & 0x40) {
        func_80016A50(&it->unkA, 0);
        func_80016A50(&D_80059ED0->unkE, 0);
        func_80016A50(&D_80059ED0[-1].unkA, 0xFF);
        func_80016A50(&D_80059ED0[-1].unkE, 0xFF);
        func_8001F138(0x26);
    } else if (buttons & 1) {
        delta = -1;
    } else if (buttons & 2) {
        delta = 1;
    }
    if (delta != 0) {
        func_8001F138(0x25);
    }
    D_80059ED0->unk8 = (u32)(D_80059ED0->unk8 + delta + 3) % 3;
}
