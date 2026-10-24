typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Ent_80025308 {
    u8 pad_00[0x10];
    s16 unk10;
    s16 unk12;
    s16 unk14;
    s16 unk16;
    u8 pad_18[0x210 - 0x18];
};
struct Grid_800581F0 {
    u8 pad_00[0xC];
    s16 unk0C;
    s16 unk0E;
    u8 pad_10[0x2000 - 0x10];
    struct Ent_80025308 ents[1];
};
struct Obj_80025308 {
    u8 pad_00[0x2C];
    f32 unk2C;
    u8 pad_30[0x4];
    f32 unk34;
    u8 pad_38[0x13E - 0x38];
    u8 unk13E;
    u8 unk13F;
    u8 unk140;
    u8 unk141;
};
extern s8 D_800581E9;
extern struct Grid_800581F0 *D_800581F0;
extern void func_80005528(struct Obj_80025308 *, s32);
extern void func_80005500(s32, struct Obj_80025308 *);

void func_80025308(struct Obj_80025308 *obj) {
    struct Grid_800581F0 *base = D_800581F0;
    u8 *e = (u8 *)base + D_800581E9 * 0x210;

    obj->unk2C = ((base->unk0E - *(s16 *)(e + 0x2016)) - 2) * 600 + 300;
    obj->unk34 = (base->unk0C - *(s16 *)(e + 0x2014)) * 600 + 300;
    obj->unk13E = 0;
    obj->unk13F = 0;
    obj->unk140 = 0;
    obj->unk141 = 0;
    func_80005528(obj, D_800581E9);
    func_80005500(0, obj);
}
