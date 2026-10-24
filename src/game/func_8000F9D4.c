/* Target: func_8000F9D4  VRAM 0x8000F9D4  ROM 0x105D4
 * Size: 0x114  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 */
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

struct Obj_8000F9D4 {
    u8 pad_00[0x2C];
    f32 unk_2C;
    f32 unk_30;
    f32 unk_34;
    u8 pad_38[0x128 - 0x38];
    s32 unk_128;
};

void func_8000F9D4(struct Obj_8000F9D4 *obj, f32 x, f32 y, f32 z, f32 *out) {
    Mtx_f rot;
    Mtx_f m;
    Mtx_f trans;

    func_80047790(trans, obj->unk_2C, obj->unk_30, obj->unk_34);
    func_80047230(rot, (f32)((obj->unk_128 + 180) % 360), 0.0f, 1.0f, 0.0f);
    func_80046B70(rot, trans, m);
    func_80046C4C(m, x, y, z, &out[0], &out[1], &out[2]);
}
