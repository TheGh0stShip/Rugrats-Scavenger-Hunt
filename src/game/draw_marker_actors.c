/* func_800344A8: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct Vec3f { f32 x, y, z; } Vec3f;
typedef struct Actor_800197C8 {
    u8 pad_00[0x2C];
    u8 unk2C[1]; /* 0x2C: embedded sub-object */
} Actor_800197C8;
extern void *func_8001E630(void);
extern Actor_800197C8 *func_800197C8(s32 id);
extern void func_8000C3AC(void *obj, Vec3f *pos);
extern void func_80047A38(void *mtx, f32 rx, f32 ry, f32 rz, f32 scale, f32 x, f32 y, f32 z);
extern void *func_8000BDFC(void *, Vec3f, void *, void *, s32);
extern void func_8000C674(void *obj);
extern Vec3f D_80054A0C[3];
extern Vec3f D_80059D48;
extern u8 D_80059CF0[];

void func_800344A8(void) {
    s32 i;
    Vec3f pos;
    void *mtx;

    for (i = 0; i < 3; i++) {
        mtx = func_8001E630();
        pos = D_80054A0C[i];
        func_8000C3AC(func_800197C8(i + 0x1A2)->unk2C, &pos);
        func_80047A38(mtx, 0.0f, 0.0f, 0.0f, 1.0f, pos.x, pos.y, pos.z);
        func_8000BDFC(func_800197C8(i + 0x1A2)->unk2C, D_80059D48, D_80059CF0, mtx, 0x20000000);
        func_8000C674(func_800197C8(i + 0x1A2)->unk2C);
    }
}
