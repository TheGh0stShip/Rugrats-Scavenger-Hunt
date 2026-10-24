typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef float f32;

struct Obj_80054B08 {
    /* 0x00 */ f32 x;
    /* 0x04 */ f32 y;
    /* 0x08 */ f32 z;
    /* 0x0C */ u8 pad_0C[0x28];
    /* 0x34 */ s32 unk34;
    /* 0x38 */ s16 unk38;
    /* 0x3A */ u8 pad_3A[2];
}; /* size 0x3C */

extern s32 D_80054AB0;
extern struct Obj_80054B08 D_80054B08[];
extern void *D_8005B33C;
typedef struct Vec3f_58F0 { f32 x, y, z; } Vec3f_58F0;
extern Vec3f_58F0 D_80059D48;
extern u8 D_80059CF0[];
extern s16 D_800C3E44;

extern void *func_8001E630(void);
extern void func_80047A38(void *, f32 rx, f32 ry, f32 rz, f32 scale, f32 x, f32 y, f32 z);
extern u8 *func_80019788(s32 id);
extern void *func_8000BDFC(void *, Vec3f_58F0, void *, void *, s32);

void func_800358F0(void) {
    s32 i;
    struct Obj_80054B08 *obj;
    f32 z;


    for (i = 0; i < D_80054AB0; i++) {
        obj = &D_80054B08[i];
        D_8005B33C = func_8001E630();
        func_80047A38(D_8005B33C, 0.0f, obj->unk38, 0.0f, 1.0f, obj->x, obj->y,
                      obj->z + ((i == D_800C3E44) ? 800.0f : 0.0f));
        func_8000BDFC(func_80019788(obj->unk34) + 0x2C, D_80059D48, D_80059CF0, D_8005B33C, 0);
    }
}
