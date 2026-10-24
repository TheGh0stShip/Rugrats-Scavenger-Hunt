typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Obj_80059DD0 {
    s32 unk0;
    u8 pad_04[4];
    s16 unk8;
    u8 unkA[4];
    u8 unkE[1];
};

struct State_80052F18 {
    s32 mode;
    u32 unk4;
    u32 unk8;
};

extern struct Obj_80059DD0 D_80059DD0;
extern struct Obj_80059DD0 *D_80059ECC;
extern s32 D_800533D8;
extern s32 D_800533DC;
extern u32 D_800533E0;
extern struct State_80052F18 D_80052F18;
extern void func_80016A50(u8 *, s32);
extern void func_8001ECE4(s32, s32);

void func_80019140(void) {
    D_800533D8 = -1;
    D_800533DC = 0;
    D_80059ECC = &D_80059DD0;
    D_80059DD0.unk0 = 1;
    D_80059DD0.unk8 = 0;
    D_80059DD0.unkA[0] = 0;
    D_80059DD0.unkE[0] = 0;
    func_80016A50(D_80059DD0.unkA, 0xFF);
    func_80016A50(D_80059ECC->unkE, 0xFF);
    D_800533E0 = D_80052F18.unk4;
    D_800533E0 = D_80052F18.unk4 + ((D_80052F18.unk8 * 3) >> 1);
    func_8001ECE4(0x200, 0x38);
}

