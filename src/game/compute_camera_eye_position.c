/* func_8000FB08  VRAM 0x8000FB08  ROM 0x10708  Size: 0x110
 * Compute the camera eye position from orbit angles and distance.
 * SN GCC 2.8 -O2 with reviewed scheduling normalization. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef f32 Mtx_f[4][4];
extern void func_80047790(Mtx_f m, f32 x, f32 y, f32 z);
extern void func_80047230(Mtx_f m, f32 a, f32 x, f32 y, f32 z);
extern void func_80046B70(Mtx_f a, Mtx_f b, Mtx_f out);
extern void func_80046C4C(Mtx_f m, f32 x, f32 y, f32 z, f32 *ox, f32 *oy, f32 *oz);

struct Obj_8000FB08 {
    u8 pad_00[0x28];
    f32 unk_28;
    f32 unk_2C;
    f32 unk_30;
    u8 pad_34[0xFC - 0x34];
    s32 unk_FC;
    s32 unk_100;
    s32 unk_104;
};

typedef struct { f32 x, y, z; } Vec3f;

void func_8000FB08(struct Obj_8000FB08 *obj, Vec3f pos) {
    Mtx_f rx;
    Mtx_f ry;
    Mtx_f trans;
    Mtx_f out;

    func_80047790(trans, pos.x, pos.y, pos.z);
    func_80047230(rx, (f32)obj->unk_FC, 1.0f, 0.0f, 0.0f);
    func_80047230(ry, (f32)obj->unk_100, 0.0f, 1.0f, 0.0f);
    func_80046B70(rx, ry, out);
    func_80046B70(out, trans, out);
    func_80046C4C(out, 0.0f, 0.0f, (f32)obj->unk_104, &obj->unk_28, &obj->unk_2C, &obj->unk_30);
}
