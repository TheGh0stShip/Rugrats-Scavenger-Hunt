/* Target: func_80017144  VRAM 0x80017144  ROM 0x17D44
 * Size: 0x358  Compiler: SN GCC 2.8 (-O2), reviewed scheduling normalization
 * Empty asm statements emit no code; they stop post-reload CSE from reusing
 * registers that hold constants or the table address.
 */
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
    u8 pad_12[0x2];
    s32 unk14;
};
struct Tbl_80052F18 {
    s32 unk0;
    u8 pad_04[0x28];
    s32 unk2C;
    s32 unk30;
};
extern struct Entry_80059DD0 D_80059DD0[];
extern struct Entry_80059DD0 *D_80059ECC;
extern struct Entry_80059DD0 *D_80059ED0;
extern s32 D_800533D8;
extern s32 D_800533DC;
extern struct Tbl_80052F18 D_80052F18;
extern s32 D_80061E54;
extern void func_80016A50(u8 *, s16);
extern int func_8001F138(s32);
extern void func_8001BED8(s32);
extern void func_8001F0A8(s32, s32);
extern void func_8001F408(s32);

void func_80017144(s32 buttons) {
    struct Entry_80059DD0 *e = D_80059ED0;
    s32 dy = 0;
    s32 dx = 0;
    s32 i;
    struct Tbl_80052F18 *t;
    s32 eight;
    s32 full;

    if (e->unk4 == 1) {
        return;
    }
    if (e->unk4 != 0) {
        if (e->unk4 == 2) {
            if (e->unk14 == 0x29) {
                func_8001BED8(0x29);
            } else if (e->unk14 == 10) {
                s32 n = D_800533D8 + 1;
                D_800533DC = n;
                D_80059ECC = &D_80059DD0[n];
                D_80059ECC->unkA[0] = 0;
                D_80059DD0[n].unk0 = 10;
                D_80059DD0[n].unk8 = 0;
                D_80059ECC->unkE[0] = 0;
                func_80016A50(D_80059ECC->unkA, 0xFF);
                func_80016A50(D_80059ECC->unkE, 0xFF);
            } else {
                e->unk4 = 0;
            }
        }
        return;
    }

    if (buttons & 0x40) {
        e->unk14 = 0x29;
        func_80016A50(e->unkA, 0);
        func_80016A50(D_80059ED0->unkE, 0);
    } else if (buttons & 0x50) {
        if ((buttons & 0x10) && e->unk8 == 2) {
            e->unk14 = 10;
            for (i = 0; i <= D_800533DC; i++) {
                func_80016A50(D_80059DD0[i].unkA, 0);
                func_80016A50(D_80059DD0[i].unkE, 0);
            }
        }
    } else if (buttons & 4) {
        dy = -1;
    } else if (buttons & 8) {
        dy = 1;
    } else if (buttons & 2) {
        dx = 1;
    } else if (buttons & 1) {
        dx = -1;
    }

    if (dy != 0 || dx != 0) {
        func_8001F138(0x25);
    }
    {
    struct Entry_80059DD0 *f = D_80059ED0;
    f->unk8 = (u32)(f->unk8 + dy + 3) % 3;
    }
    if (dx == 0) {
        return;
    }
    switch (D_80059ED0->unk8) {
        case 0:
            t = &D_80052F18;
            __asm__ ("" : "+r"(t));
            t->unk2C = (t->unk2C + 9 + dx) % 9;
            func_8001F0A8(t->unk2C << 12, t->unk30 << 12);
            break;
        case 1:
            t = &D_80052F18;
            __asm__ ("" : "+r"(t));
            t->unk30 = (t->unk30 + 9 + dx) % 9;
            func_8001F0A8(t->unk2C << 12, t->unk30 << 12);
            break;
        default:
            return;
    }
    eight = 8;
    __asm__ __volatile__("" : "+r"(eight));
    full = 0x80;
    func_8001F408(((full - (eight - t->unk2C) * 16) * 255) & 0xFFF0);
    eight -= t->unk2C;
    full -= eight * 16;
    D_80061E54 = full * 255;
    { s32 four; __asm__("" : "=r"(four) : "0"(4)); func_8001F138(four); }
}
