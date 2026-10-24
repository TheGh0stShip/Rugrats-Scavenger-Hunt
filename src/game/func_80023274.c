typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Obj_80023274 {
    u8 pad_000[0x18C];
    u32 unk18C;
    u8 pad_190[0x16];
    s8 unk1A6;
    s8 unk1A7;
};

extern s32 func_8001B5BC(void);
extern s32 func_8001B5EC(void);
extern s32 func_8001BF00(void);

void func_80023274(struct Obj_80023274 *obj) {
    s32 a = func_8001B5BC();
    s32 b = func_8001B5EC();
    s32 d;
    s32 lim;

    if (func_8001BF00() != 0x23) {
        lim = obj->unk1A7 - 20;
        if (lim < a - b) {
            obj->unk18C |= 1;
        }
        d = obj->unk1A7 - b;
        if (d < a && d < lim) {
            obj->unk18C |= 2;
        } else {
            obj->unk18C |= 0x10;
        }
    } else {
        d = a - b;
        lim = obj->unk1A6 - 20;
        if (lim < d) {
            obj->unk18C |= 2;
        }
        if (d < lim && d < a) {
            obj->unk18C |= 1;
        } else {
            obj->unk18C |= 0x10;
        }
    }
}

