typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
typedef double f64;

struct Obj_80052F04 {
    u8 pad_00[0x28];
    f32 unk_28;
    f32 unk_2C;
    f32 unk_30;
    u8 pad_34[0x58 - 0x34];
    f32 unk_58;
    f32 unk_5C;
    f32 unk_60;
};

extern struct Obj_80052F04 *D_80052F04;
extern s32 D_80058678;
extern f64 func_80038C20(f64 y, f64 x);

void func_80010B0C(void) {
    struct Obj_80052F04 *o = D_80052F04;
    s32 dx = o->unk_58 - o->unk_28;
    s32 dz = o->unk_60 - o->unk_30;
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
    deg = (ang * 180.0f) / 3.141592654 - 30.0;
    D_80058678 = deg;
    if (deg < 0) {
        D_80058678 = deg + 360;
    }
}
