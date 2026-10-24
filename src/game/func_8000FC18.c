/* Target: func_8000FC18  VRAM 0x8000FC18  ROM 0x10818
 * Size: 0x120  Compiler: KMC GCC 2.7.2 (-O2)
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

typedef struct {
    f32 x, y, z;
} Vec3f;


void func_8000FC18(f32 *out, s32 pitch, s32 yaw, s32 dist, Vec3f *pos) {
    Mtx_f rx;
    Mtx_f ry;
    Mtx_f trans;
    Mtx_f m;

    func_80047790(trans, pos->x, pos->y, pos->z);
    func_80047230(rx, (f32)pitch, 1.0f, 0.0f, 0.0f);
    func_80047230(ry, (f32)yaw, 0.0f, 1.0f, 0.0f);
    func_80046B70(rx, ry, m);
    func_80046B70(m, trans, m);
    func_80046C4C(m, 0.0f, 0.0f, (f32)dist, &out[0], &out[1], &out[2]);
}
