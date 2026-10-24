/* Target: func_80007D00  VRAM 0x80007D00  ROM 0x8900
 * Size: 0x1B0  Pipeline: SN GCC 2.8 -O2 -G0 (normalized return), .rdata at 0x800005D0 (shares file-level .rdata with func_80007A28)
 * Confidence: medium */
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef float f32;
typedef double f64;

typedef struct {
    f32 x, y, z;
} Vec3f;

struct Off_80052F90 {
    f32 x;
    f32 z;
};
struct Res_80007D00 {
    u8 pad_00[0x2C];
    s32 unk2C;
};
struct Obj_80007D00 {
    u8 pad_000[0x2C];
    Vec3f pos;
    u8 unk38[0x28];
    f32 mtx[2][4][4];
    s32 unkE0;
    s32 unkE4;
    u8 pad_0E8[0xC];
    s32 unkF4;
    u8 pad_0F8[0x50];
    f32 unk148;
    u8 pad_14C[0x4];
    s16 unk150;
};

extern s32 D_80052F18;
extern s32 D_80054358;
extern struct Off_80052F90 D_80052F90[];

extern void func_80006B38(Vec3f *out, s32 id, s32 arg2, f32 t);
extern f32 func_80006DCC(f32 arg0);
extern void func_8000CF10(void *, float, float, float, float, float, float, float);
extern struct Res_80007D00 *func_800197C8(s32 arg0);
extern void func_80007EB0(void *, void *, void *, void *, int);
extern void func_8000C674(s32 *arg0);

/* Shared file-level .rdata word owned by func_80007A28 (600.0f at 0x800005D0). */
static const f32 D_800005D0 = 600.0f;

void func_80007D00(struct Obj_80007D00 *obj) {
    Vec3f v;

    if (D_80052F18 == 2) {
        func_80006B38(&v, obj->unk150, 0, obj->unk148);
        func_8000CF10(obj->mtx[D_80054358], 0.0f, func_80006DCC(obj->unk148) - 90.0, 0.0f,
                      0.5f, v.x, v.y, v.z);
        func_80007EB0(&func_800197C8(obj->unkE0)->unk2C, &v, obj->unk38, obj->mtx[D_80054358], 0);
        func_8000C674(&func_800197C8(obj->unkE0)->unk2C);
    } else {
        func_8000CF10(obj->mtx[D_80054358], 0.0f, 180.0f, 0.0f, 1.0f,
                      obj->pos.x + D_80052F90[obj->unkF4].x, obj->pos.y,
                      obj->pos.z + D_80052F90[obj->unkF4].z);
        func_80007EB0(&func_800197C8(obj->unkE4)->unk2C, &obj->pos, obj->unk38, obj->mtx[D_80054358], 0);
    }
}
