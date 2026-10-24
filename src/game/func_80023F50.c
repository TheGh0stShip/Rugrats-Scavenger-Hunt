typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

struct Obj_80023F50 {
    u8 pad_000[0x118];
    u32 unk118;
    u8 pad_11C[0x14];
    s32 unk130;
    u8 pad_134[0x8];
    u8 unk13C;
    u8 unk13D;
    u8 unk13E;
    u8 unk13F;
};

extern s32 D_80052F18;
extern s32 D_800529F8;
extern s32 D_80057D5C;
extern s16 D_80057DA0;
extern u8 D_80057D68[][2];
extern struct Obj_80023F50 *func_80009140(void);
extern struct Obj_80023F50 *func_800055A4(s32);
extern void func_80009554(struct Obj_80023F50 *, struct Obj_80023F50 *);

s32 func_80023F50(struct Obj_80023F50 *obj) {
    s32 ret = 0;
    s32 count = 0;
    s32 flag = 0;
    s32 i;

    if (D_80052F18 == 2 || (obj->unk118 & 0x20)) {
        return 0;
    }
    if (obj->unk118 & 8) {
        flag = 1;
        obj->unk118 &= ~8;
    }
    if (D_80052F18 == 1 && !flag) {
        return 0;
    }
    if (D_800529F8 == obj->unk130 && !flag) {
        return ret;
    }
    if ((obj->unk13C != func_80009140()->unk13C
         || obj->unk13D != func_80009140()->unk13D) && !flag) {
        return ret;
    }
    if (obj->unk118 & 2) {
        return ret;
    }
    for (i = 0; i < D_80057D5C; i++) {
        if (func_800055A4(i)->unk13C == obj->unk13C
            && func_800055A4(i)->unk13D == obj->unk13D) {
            count++;
        }
    }
    if (count >= 2 && !flag) {
        return ret;
    }
    ret = 1;
    func_80009554(func_80009140(), obj);
    obj->unk13E = D_80057D68[D_80057DA0 - 1][0];
    obj->unk13F = D_80057D68[D_80057DA0 - 1][1];
    return ret;
}
