/* Target: func_80010BD8  VRAM 0x80010BD8  ROM 0x117D8
 * Size: 0xE4  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
typedef double f64;

struct Obj_80052F04 {
    u8 pad_00[0x58];
    f32 unk_58;
    f32 unk_5C;
    f32 unk_60;
};

extern struct Obj_80052F04 *D_80052F04;
extern s32 D_80058678;
extern f64 func_80038C20(f64 y, f64 x);

s32 func_80010BD8(s32 x, s32 z, s32 *out) {
    struct Obj_80052F04 *o = D_80052F04;
    s32 dx = o->unk_58 - (f32)x;
    s32 dz = o->unk_60 - (f32)z;
    f32 ang;
    s32 deg;

    if (dx != 0) {
        ang = func_80038C20(dz, dx);
    } else {
        ang = -1.5707964f;
        if (dz >= 0) {
            ang = 1.5707964f;
        }
    }
    deg = (ang * 180.0f) / 3.141592654;
    if (deg < 0) {
        deg += 360;
    }
    if (deg < D_80058678) {
        deg += 360;
    }
    *out = deg - D_80058678;
    return *out < 60;
}
