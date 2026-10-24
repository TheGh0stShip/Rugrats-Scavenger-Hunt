/* Target: func_80018F04  VRAM 0x80018F04  ROM 0x19B04
 * Size: 0xF8  Compiler: SN GCC 2.8 (-O2), reviewed scheduling normalization */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

/* 0x18-byte menu entry; D_80059DB8 is the table base, D_80059DD0 = entry 1 */
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

extern struct Item_80059DB8 *D_80059ED0;
extern struct Item_80059DB8 D_80059DD0[];
extern s32 D_800533D8;
extern s32 D_800533DC;
extern void func_80016A50(u8 *, s32);

void func_80018F04(s32 buttons) {
    struct Item_80059DB8 *it = D_80059ED0;
    s32 n;
    u8 *p;

    if (it->unk4 == 1) {
    } else if (it->unk4 != 0) {
        if (it->unk4 == 2) {
            if (it->unkB == 0) {
                n = D_800533D8;
                if (n != 0) {
                    D_800533DC = n - 1;
                    D_80059DD0[n].unk0 = -1;
                }
            } else {
                it->unk4 = 0;
            }
        }
    } else {
            p = &it->unkA;
            if (buttons & 0x40) {
                func_80016A50(p, 0);
                func_80016A50(&D_80059ED0->unkE, 0);
                func_80016A50(&D_80059ED0[-1].unkA, 0xFF);
                func_80016A50(&D_80059ED0[-1].unkE, 0xFF);
                func_80016A50(&D_80059ED0[-2].unkA, 0x7F);
                func_80016A50(&D_80059ED0[-2].unkE, 0xFF);
            }
    }
}
