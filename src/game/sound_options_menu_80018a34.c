/* Target: func_80018A34  VRAM 0x80018A34  ROM 0x19634
 * Size: 0x3A8  Compiler: SN GCC 2.8 (-O2), reviewed scheduling normalization
 * Empty asm statements emit no code; they stop post-reload CSE from reusing
 * registers that hold constants or the table address.
 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Item_80059DB8 {
    s32 unk0;
    u32 unk4;
    s16 unk8;
    u8 unkA;
    u8 unkB;
    u8 pad_0C[2];
    u8 unkE;
    u8 pad_0F[9];
};

struct State_80052F18 {
    s32 mode;
    u8 pad_04[4];
    s32 unk8;
    u8 pad_0C[0x20];
    s32 unk2C;
    s32 unk30;
};

extern struct Item_80059DB8 D_80059DD0[];
extern struct Item_80059DB8 D_80059DE8[];
extern struct Item_80059DB8 *D_80059ECC;
extern struct Item_80059DB8 *D_80059ED0;
extern s32 D_800533D8;
extern s32 D_800533DC;
extern struct State_80052F18 D_80052F18;
extern s32 D_80061E54;
extern void func_80016A50(u8 *, s32);
extern int func_8001F138(s32);
extern void func_8001F0A8(s32, s32);
extern void func_8001F408(s32);

void func_80018A34(s32 buttons) {
    struct Item_80059DB8 *it = D_80059ED0;
    s32 delta = 0;
    s32 change = 0;
    s32 i;
    s32 v;
    struct State_80052F18 *st;
    struct State_80052F18 *st2;
    s32 eight;
    s32 full;

    switch (it->unk4) {
        case 1:
            break;
        case 2:
            if (it->unkB == 0) {
                if (D_80059ECC->unk0 == 8) {
                    D_800533DC = D_800533D8 + 1;
                } else {
                    D_800533DC = D_800533D8 - 1;
                }
            } else {
                it->unk4 = 0;
            }
            break;
        case 0:
            if (buttons & 0x40) {
                func_80016A50(&it->unkA, 0);
                func_80016A50(&D_80059ED0->unkE, 0);
                func_80016A50(&D_80059ED0[-1].unkA, 0xFF);
                func_80016A50(&D_80059ED0[-1].unkE, 0xFF);
                func_8001F138(0x26);
            } else if (buttons & 0x10) {
                if (it->unk8 == 3) {
                    D_80059ECC = &D_80059DD0[D_800533DC + 1];
                    D_80059ECC->unk0 = 8;
                    D_80059ECC->unkA = 0;
                    D_80059ECC->unkE = 0;
                    func_80016A50(&D_80059ECC->unkA, 0xFF);
                    func_80016A50(&D_80059ECC->unkE, 0xFF);
                    for (i = 0; i <= D_800533D8; i++) {
                        func_80016A50(&D_80059DD0[i].unkA, 0);
                        func_80016A50(&D_80059DD0[i].unkE, 0);
                    }
                }
                func_8001F138(0x27);
            } else if (buttons & 4) {
                delta = -1;
            } else if (buttons & 8) {
                delta = 1;
            } else if (buttons & 2) {
                change = 1;
            } else if (buttons & 1) {
                change = -1;
            }
            if (delta != 0 || change != 0) {
                func_8001F138(0x25);
            }
            v = (D_80059ED0->unk8 + delta + 4) & 3;
            D_80059ED0->unk8 = v;
            if (change != 0) {
                switch ((s16)v) {
                    case 0:
                        st = &D_80052F18;
                        __asm__ ("" : "+r"(st));
                        st->unk2C = (st->unk2C + 9 + change) % 9;
                        func_8001F0A8(st->unk2C << 12, st->unk30 << 12);
                        goto volume;
                    case 1:
                        st = &D_80052F18;
                        __asm__ ("" : "+r"(st));
                        st->unk30 = (st->unk30 + 9 + change) % 9;
                        func_8001F0A8(st->unk2C << 12, st->unk30 << 12);
                    volume:
                        eight = 8;
                        __asm__ __volatile__("" : "+r"(eight));
                        full = 0x80;
                        func_8001F408(((full - (eight - st->unk2C) * 16) * 255) & 0xFFF0);
                        eight -= st->unk2C;
                        full -= eight * 16;
                        D_80061E54 = full * 255;
                        break;
                    case 2:
                        st2 = &D_80052F18;
                        st2->unk8 = (st2->unk8 + 2 + change) % 2;
                        break;
                }
            }
            break;
    }
}
