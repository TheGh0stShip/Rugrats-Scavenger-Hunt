typedef unsigned char u8;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Obj_80009BE0 {
    u8 pad_000[0x150];
    s16 unk150;
    u8 pad_152[0x5A];
    s32 unk1AC[4];
};
extern s32 func_8000DFE4(s32 arg0);
extern s16 func_8000E02C(s32 arg0, s32 arg1);

s32 func_80009BE0(struct Obj_80009BE0 *obj) {
    s32 i;
    s32 id;

    for (i = 0; i < func_8000DFE4(obj->unk150); i++) {
        id = func_8000E02C(obj->unk150, i);
        if ((u32)(id - 400) < 4 && (obj->unk1AC - 400)[id] != id) {
            return i;
        }
    }
    return -1;
}
