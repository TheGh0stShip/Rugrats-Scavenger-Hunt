
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef float f32;
typedef struct { f32 x, y, z; } Vec3f;

struct Obj_80057F70 {
    u8 pad_000[0x204];
    s32 unk204;
    u8 pad_208[0x1C];
    s32 unk224;
};
extern struct Obj_80057F70 D_80057F70;

extern s32 func_8001E668(void);
extern s32 func_8001EEAC(void);
extern void func_8001E718(s32 arg0);
extern void func_8001EEBC(void);
extern void func_8001ECE4(s32 arg0, s32 arg1);
extern s32 func_8001EFC4(void);
extern void *func_8001EED0(void);

s32 func_80009C8C(s32 target) {
    struct Obj_80057F70 *p;

    if (func_8001E668() != 0 && func_8001EEAC() != 0x210) {
        func_8001E718(10);
        func_8001EEBC();
        func_8001ECE4(0x210, 0x38);
    }
    if (func_8001EEAC() == 0x210 && func_8001EFC4() == 0) {
        func_8001EED0();
    }
    {
        struct Obj_80057F70 *q = &D_80057F70;
        if (q->unk224 != 0) {
            if (--q->unk224 != -1) {
                return 0;
            }
        }
    }
    p = &D_80057F70;
    p->unk224 = 20;
    if (p->unk204 == target) {
        p->unk224 = 60;
        return 0x10;
    }
    if (p->unk204 < target) {
        return 1;
    }
    return 2;
}
