typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Actor_800222E8 {
    u8 pad_00[0xF8];
    u32 kind;
    u8 pad_FC[0x134 - 0xFC];
    s8 sameTile[8];
    union {
        struct { u8 tx; u8 ty; u8 pad[2]; } b;
        u32 w;
    } tile;
    u8 pad_140[0x150 - 0x140];
    s16 room;
};

struct Block_80057F70 {
    u8 pad_000[0x204];
    s32 unk204;
    u8 pad_208[0x214 - 0x208];
    s32 unk214;
    u8 pad_218[0x224 - 0x218];
    s32 unk224;
};

extern s32 D_80057D58;
extern s32 D_80057D5C;
extern struct Actor_800222E8 *D_80057DA8[];
extern u8 D_800539B0[];
extern s32 D_80052F18;
extern s32 D_800529F8;
extern struct Block_80057F70 D_80057F70;
extern struct Actor_800222E8 *func_800055C0(u8);
extern struct Actor_800222E8 *func_800055A4(s32);
extern s32 func_8003A840(void);
extern void func_80024480(struct Actor_800222E8 *);
extern void func_80024534(struct Actor_800222E8 *);
extern s32 func_8000DF14(s32, s32);
extern s32 func_8000DF40(s16);
extern void func_8000DE4C(int, int);
extern s32 func_800094A4(struct Actor_800222E8 *);
extern s32 func_8000E008(s16);
extern s32 func_80009C64(struct Actor_800222E8 *, s16);
extern void func_8001BED8(s32);
extern void func_8001E6F8(s32);

s32 func_800222E8(struct Actor_800222E8 *a) {
    u8 n = 0;
    s32 hit = 0;
    u8 i = 0;
    s32 res = 0;
    u8 pick = 0;
    s32 t;

    for (; i < D_80057D58; i++) {
        if (func_800055C0(i) != a && func_800055C0(i)->tile.b.tx == a->tile.b.tx &&
            func_800055C0(i)->tile.b.ty == a->tile.b.ty) {
            return 1;
        }
    }
    for (i = 0; i < D_80057D5C; i++) {
        if ((D_80057DA8[i]->tile.w & 0xFFFF0000) == (a->tile.w & 0xFFFF0000)) {
            a->sameTile[i] = i;
            n++;
        } else {
            a->sameTile[i] = -1;
        }
    }
    if (n != 0) {
        if (pick == 0) {
            do {
                pick = func_8003A840() % D_800539B0[n];
            } while (pick == 0);
        }
        for (i = 0; i < D_80057D5C; i++) {
            if (a->sameTile[i] == -1) {
                continue;
            }
            switch (a->kind) {
            case 5:
                if (n == 1) {
                    hit = 1;
                    func_80024480(func_800055A4(a->sameTile[i]));
                    res = 3;
                }
                break;
            case 6:
                if (n == 1) {
                    hit = 1;
                    func_80024534(func_800055A4(a->sameTile[i]));
                    res = 0;
                }
                break;
            case 4:
                if ((s16)func_8000DF40(func_8000DF14(a->tile.b.ty, a->tile.b.tx)) == 0xD &&
                    D_80052F18 == 0) {
                    break;
                }
                if (n == 1) {
                    hit = func_800094A4(a);
                    if (hit != 0) {
                        res = 3;
                    }
                }
                break;
            }
        }
    }
    __asm__("" :: "r"(hit));
    if (hit != 0) {
        return res;
    }
    {
        u32 k = a->kind;
        {
            s32 c;
            __asm__("" : "=r"(c) : "0"(4));
            if (k == c) goto c4;
        }
        if (k < 4) goto out;
        if (k >= 7) goto out;
        if (n >= 2) {
            res = 1;
        }
        goto out;
c4:
        if (D_80052F18 != 0) {
            goto out;
        }
        if (D_800529F8 != 0) {
            res = 1;
            goto out;
        }
        t = (s16)func_8000DF40(func_8000DF14(a->tile.b.ty, a->tile.b.tx));
        func_8000DE4C(a->tile.b.ty, a->tile.b.tx);
        if (t != 1) {
            goto out;
        }
        if (func_8000E008(a->room) == 0) {
            res = 0;
            goto out;
        }
        D_80057F70.unk214 = t;
        D_80057F70.unk204 = func_80009C64(a, a->room);
        res = 2;
        D_80057F70.unk224 = 30;
        func_8001BED8(0x18);
        func_8001E6F8(0xA);
    }
out:
    return res;
}
