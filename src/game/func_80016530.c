/* Target: func_80016530  VRAM 0x80016530  ROM 0x17130
 * Size: 0x180  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Pad_80058B58 {
    u8 pad_00[0x2];
    u16 button;
    u16 trig;
    s8 stickX;
    s8 stickY;
    u8 pad_08[0x2];
};
extern struct Pad_80058B58 D_80058B58[];
extern f32 D_800012D0;
extern f32 D_800012D4;

s32 func_80016530(s32 idx) {
    struct Pad_80058B58 *p = &D_80058B58[idx];
    s32 sx = p->stickX;
    u16 btn = p->button;
    s32 sy = p->stickY;
    s32 res = 0;
    u16 t;

    if ((D_800012D0 < (f32)sx && sy == 0) || (btn & 0x100)) {
        res |= 2;
    } else if (((f32)sx < D_800012D4 && sy == 0) || (btn & 0x200)) {
        res |= 1;
    } else if (((f32)sy < D_800012D4 && sx == 0) || (btn & 0x400)) {
        res |= 8;
    } else if ((D_800012D0 < (f32)sy && sx == 0) || (btn & 0x800)) {
        res |= 4;
    }

    t = D_80058B58[idx].trig;
    if (t & 0x8000) res |= 0x10;
    if (t & 0x4000) res |= 0x40;
    if (t & 0x2000) res |= 0x200;
    if (t & 0x1000) res |= 0x20;
    if (t & 0x2) res |= 0x400;
    if (t & 0x1) res |= 0x800;
    if (t & 0x8) res |= 0x1000;
    if (t & 0x4) res |= 0x2000;
    if (t & 0x20) res |= 0x80;
    if (t & 0x10) res |= 0x100;
    return res;
}
