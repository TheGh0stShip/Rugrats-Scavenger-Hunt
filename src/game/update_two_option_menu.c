typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct Entry_80059DD0 {
    s32 unk0;
    s32 unk4;
    s16 unk8;
    u8 unkA[4];   /* 0x0A */
    u8 unkE[4];   /* 0x0E */
    u8 pad_12[0x2];
    s32 unk14;
}; /* size 0x18 */
extern struct Entry_80059DD0 D_80059DD0[];
extern struct Entry_80059DD0 *D_80059ECC;
extern struct Entry_80059DD0 *D_80059ED0;
extern s32 D_800533D8;
extern s32 D_800533DC;
extern void func_80016A50(u8 *, s16);
extern int func_8001F138(s32);

void func_8001757C(s32 buttons) {
    struct Entry_80059DD0 *e = D_80059ED0;
    s32 dir = 0;

    if (e->unk4 == 1) {
        return;
    }
    if (e->unk4 != 0) {
        if (e->unk4 == 2) {
            if (e->unkA[1] == 0) {
                if (D_800533D8 != 0) {
                    D_800533DC = D_800533D8 - 1;
                }
            } else {
                e->unk4 = 0;
            }
        }
        return;
    }
    if (buttons & 0x10) {
        s32 n = D_800533D8 + 1;
        D_800533DC = n;
        D_80059ECC = &D_80059DD0[n];
        switch (e->unk8) {
            case 0:
                D_80059DD0[n].unk0 = 5;
                break;
            case 1:
                D_80059DD0[n].unk0 = 4;
                D_80059DD0[n].unk8 = 0;
                break;
        }
        D_80059ECC->unkA[0] = 0;
        D_80059ECC->unkE[0] = 0;
        func_80016A50(D_80059ECC->unkA, 0xFF);
        func_80016A50(D_80059ECC->unkE, 0xFF);
        func_80016A50(D_80059ED0->unkA, 0x7F);
        func_8001F138(0x27);
    } else if (buttons & 1) {
        dir = -1;
    } else if (buttons & 2) {
        dir = 1;
    }
    D_80059ED0->unk8 = (u32)(D_80059ED0->unk8 + dir + 2) % 2;
    if (dir != 0) {
        func_8001F138(0x25);
    }
}
