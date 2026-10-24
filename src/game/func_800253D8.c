typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

struct Obj_800253D8 {
    u8 pad_00[0xF0];
    s32 unkF0;
    u8 pad_F4[0x118 - 0xF4];
    s32 unk118;
    u8 pad_11C[0x156 - 0x11C];
    s16 unk156;
};

extern struct Obj_800253D8 *D_80052F04;
extern s32 func_8003A840(void);
extern void func_80005500(s32, struct Obj_800253D8 *);
extern void func_8000E680(struct Obj_800253D8 *);

void func_800253D8(struct Obj_800253D8 *obj) {
    if (func_8003A840() & 1) {
        obj->unk156 = func_8003A840() % 3 + 1;
    } else {
        obj->unk156 = func_8003A840() % 2 - 2;
    }
    func_80005500(7, obj);
    obj->unk118 |= 0x8000;
    func_8000E680(D_80052F04);
    D_80052F04->unkF0 = 0;
}
