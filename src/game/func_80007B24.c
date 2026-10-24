typedef unsigned char u8;
typedef int s32;
typedef float f32;

typedef struct {
    f32 x, y, z;
} Vec3f;

struct Res_80007B24 {
    u8 pad_00[0x2C];
    s32 unk2C;
};
struct Obj_80007B24 {
    u8 pad_000[0x2C];
    Vec3f pos;
    u8 pad_038[0x28];
    f32 mtx[2][4][4];
    s32 unkE0;
    u8 pad_0E4[0x40];
    s32 unk124;
};

extern s32 D_80054358;
extern struct Res_80007B24 *func_800197C8(s32 arg0);
extern void func_8000C3AC(s32 *res, Vec3f *pos);
extern void func_8000CF10(void *, float, float, float, float, float, float, float);

void func_80007B24(struct Obj_80007B24 *obj, f32 scale) {
    Vec3f v;

    v.x = obj->pos.x;
    v.y = obj->pos.y;
    v.z = obj->pos.z;
    func_8000C3AC(&func_800197C8(obj->unkE0)->unk2C, &v);
    func_8000CF10(obj->mtx[D_80054358], 0.0f, (f32)obj->unk124, 0.0f, scale, v.x, v.y, v.z);
}
