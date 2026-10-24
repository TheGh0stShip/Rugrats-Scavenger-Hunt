typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef float f32;

struct Obj_80010760 {
    u8 pad_00[0xF0];
    s32 unk_F0;
    u8 pad_F4[0x104 - 0xF4];
    s32 unk_104;
    u8 pad_108[0x114 - 0x108];
    f32 unk_114;
    u8 pad_118[4];
    f32 unk_11C;
};

struct Pos_80005544 {
    u8 pad_00[0x2C];
    f32 unk_2C;
    u8 pad_30[4];
    f32 unk_34;
};

extern struct Pos_80005544 *func_80005544(void);
extern s32 D_80057D94;
extern s8 D_800581E9;
extern u8 *D_800581F0;

void func_80010760(struct Obj_80010760 *obj) {
    struct Pos_80005544 *p;
    u8 *e;

    if (obj->unk_F0 == 0x11) {
        obj->unk_F0 = 0x12;
    } else {
        obj->unk_F0 = 0x16;
    }
    D_80057D94 = 1;
    p = func_80005544();
    e = D_800581F0 + D_800581E9 * 0x210;
    obj->unk_114 = p->unk_2C + (f32)(*(s16 *)(e + 0x2016) * 600);
    obj->unk_11C = p->unk_34 + (f32)(*(s16 *)(e + 0x2014) * 600);
    obj->unk_104 = 0xC80;
}
