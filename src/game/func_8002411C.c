typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Obj_8002411C {
    u8 pad_000[0x118];
    u32 unk118;
    u8 pad_11C[0x20];
    u8 unk13C;
    u8 unk13D;
    u8 pad_13E[0x12];
    s16 unk150;
    u8 pad_152[0x55];
    s8 unk1A7;
};

extern s32 D_80057D5C;
extern s32 D_80052F18;
extern struct Obj_8002411C *func_80020680(void);
extern struct Obj_8002411C *func_800206A8(void);
extern struct Obj_8002411C *func_80009140(void);
extern struct Obj_8002411C *func_800055A4(s32);
extern void func_80024480(struct Obj_8002411C *);
extern void func_80024534(struct Obj_8002411C *);
extern void func_800069C8(s32, struct Obj_8002411C *, s32);
extern s32 func_8000E008(s16);
extern void func_80009554(struct Obj_8002411C *, struct Obj_8002411C *);

s32 func_8002411C(struct Obj_8002411C *obj) {
    s32 ret = 0;
    s32 count = 0;
    s32 i;

    if (obj->unk13C == func_80020680()->unk13C && obj->unk13D == func_80020680()->unk13D) {
        func_80024480(obj);
        ret = 1;
    }
    for (i = 0; i < D_80057D5C; i++) {
        if (func_800055A4(i)->unk13C == obj->unk13C && func_800055A4(i)->unk13D == obj->unk13D) {
            count++;
        }
    }
    if (ret) {
        return ret;
    }
    if (count < 2 && obj->unk13C == func_800206A8()->unk13C && obj->unk13D == func_800206A8()->unk13D) {
        func_80024534(obj);
        func_800069C8(0, obj, 1);
        ret = 1;
    }
    if (ret) {
        return ret;
    }
    if (count < 2 && D_80052F18 == 1) {
        if (obj->unk1A7 < 10) {
            if (obj->unk13C == func_80009140()->unk13C && obj->unk13D == func_80009140()->unk13D) {
                obj->unk118 |= 0x1000000;
            }
        } else if (func_8000E008(obj->unk150)
                   && obj->unk13C == func_80009140()->unk13C
                   && obj->unk13D == func_80009140()->unk13D) {
            ret = 1;
            func_80009554(func_80009140(), obj);
        }
    }
    return ret;
}

