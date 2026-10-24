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

extern struct Item_80059DB8 D_80059DB8[];
extern struct Item_80059DB8 *D_80059ECC;
extern struct Item_80059DB8 *D_80059ED0;
extern s32 D_800533D8;
extern s32 func_80011728(void);
extern void func_80016A50(u8 *, s32);

void func_80018DDC(void) {
    s32 r;
    s32 i;
    struct Item_80059DB8 *t;

    r = func_80011728();
    if (r == 2) {
        D_80059ECC = &D_80059DB8[D_800533D8];
        t = D_80059ED0;
        t->unkB = 0;
        t->unk4 = r;
        D_80059ECC->unkA = 0;
        D_80059ECC->unkE = 0;
        func_80016A50(&D_80059ECC->unkA, 0xFF);
        func_80016A50(&D_80059ECC->unkE, 0xFF);
        for (i = 0; i < D_800533D8; i++) {
            if (i == D_800533D8 - 1) {
                func_80016A50(&D_80059ECC->unkA, 0xFF);
            } else {
                func_80016A50(&D_80059DB8[i + 1].unkA, 0x7F);
            }
            func_80016A50(&D_80059DB8[i + 1].unkE, 0xFF);
        }
    }
}

