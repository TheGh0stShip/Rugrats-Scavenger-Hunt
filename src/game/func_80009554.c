typedef unsigned char u8;
typedef signed int s32;

struct Ent_80009554 {
    u8 pad_000[0x1D4];
    s32 unk1D4;
};

struct Obj_80057F70 {
    u8 pad_000[0x214];
    s32 unk214;
    struct Ent_80009554 *unk218;
};

struct Global_80052F04 {
    u8 pad_000[0xF0];
    s32 unkF0;
};

extern struct Obj_80057F70 D_80057F70;
extern struct Global_80052F04 *D_80052F04;

extern void func_80024328(struct Ent_80009554 *ent);
extern void func_80005500(s32 arg0, struct Ent_80009554 *ent);
extern void func_8000550C(s32 arg0, struct Ent_80009554 *ent);
extern void func_80005514(struct Ent_80009554 *ent, s32 arg1);
extern void func_80005E3C(struct Ent_80009554 *a, struct Ent_80009554 *b);

void func_80009554(struct Ent_80009554 *a, struct Ent_80009554 *b) {
    func_80024328(b);
    D_80057F70.unk218 = b;
    func_80005500(8, b);
    func_8000550C(5, b);
    func_80005500(8, a);
    func_8000550C(5, a);
    func_80005514(b, 1);
    func_80005514(a, 1);
    D_80052F04->unkF0 = 12;
    D_80057F70.unk214 = 1;
    a->unk1D4 = 0;
    func_80005E3C(a, b);
}
