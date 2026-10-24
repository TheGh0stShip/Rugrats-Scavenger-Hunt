typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Obj_80023ECC {
    u8 pad_000[0x144];
    s32 unk144;
    u8 pad_148[0x8];
    s16 unk150;
};

extern s32 D_80057D5C;
extern struct Obj_80023ECC *func_800055A4(s32);

struct Obj_80023ECC *func_80023ECC(void) {
    s32 i;
    struct Obj_80023ECC *obj;

    for (i = 0; i < D_80057D5C; i++) {
        obj = func_800055A4(i);
        if (obj->unk150 == 0 && obj->unk144 == 0x11) {
            return obj;
        }
    }
    return 0;
}

