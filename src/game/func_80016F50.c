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
extern s32 D_800533D8;
extern s32 D_800533DC;
extern void *func_80038E90(void *, s32, s32);
extern void func_80016A50(u8 *, s16);

void func_80016F50(void) {
    func_80038E90(D_80059DD0, -1, 0xF0);
    D_800533D8 = -1;
    D_800533DC = 0;
    D_80059ECC = &D_80059DD0[0];
    D_80059DD0[0].unk0 = 9;
    D_80059DD0[0].unk8 = 0;
    D_80059DD0[0].unkA[0] = 0;
    D_80059DD0[0].unkE[0] = 0;
    func_80016A50(D_80059DD0[0].unkA, 0xFF);
    func_80016A50(D_80059ECC->unkE, 0xFF);
}

