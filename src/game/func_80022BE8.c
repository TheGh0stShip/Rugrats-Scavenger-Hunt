typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32; typedef float f32;

struct Obj_80022BE8 {
    u8 pad_000[0x124];
    s32 unk124;
    u8 pad_128[0x14];
    u8 unk13C;
    u8 unk13D;
    u8 pad_13E[0x1E];
    s32 unk15C;
    s32 unk160;
    u8 pad_164[0xC];
    s16 unk170;
    s16 unk172;
    u8 unk174[1][2];
    u8 pad_176[0x16];
    u32 unk18C;
};

extern s32 func_80022060(struct Obj_80022BE8 *, u8 *);
extern s16 func_8000DF14(u8, u8);
extern s16 func_8000DFA4(s16);
extern s32 func_80005824(struct Obj_80022BE8 *, u8, u8);

void func_80022BE8(struct Obj_80022BE8 *obj) {
    volatile u8 dest[2];
    s32 angle;
    s16 idx;

    dest[0] = obj->unk15C;
    dest[1] = obj->unk160;
    idx = obj->unk172;
    if (obj->unk15C == obj->unk13C && obj->unk160 == obj->unk13D && obj->unk170 == idx) {
        obj->unk18C |= 0x10;
        return;
    }
    if (obj->unk174[idx][0] == obj->unk13C && obj->unk174[idx][1] == obj->unk13D) {
        idx = ++obj->unk172;
    }
    angle = obj->unk124 - func_80022060(obj, obj->unk174[idx]);
    if (func_8000DFA4(func_8000DF14(obj->unk13D, obj->unk13C)) != 0
        && func_80005824(obj, obj->unk13D, obj->unk13C)) {
        angle = 0;
    }
    if (angle == 0) {
        obj->unk18C |= 4;
        return;
    }
    if ((f32)angle >= 0.0f) {
        if (angle == 180) {
            goto set2;
        }
        goto check;
    } else if (-angle != 180) {
        goto check;
    }
set2:
    obj->unk18C |= 2;
    return;
check:
    if (angle == 90 || angle == -270) {
        obj->unk18C |= 2;
    } else {
        obj->unk18C |= 1;
    }
}
