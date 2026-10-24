typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
typedef struct { f32 x, y, z; } Vec3f;

struct Obj_80016434 {
    u8 pad_00[0x2C];
    Vec3f pos;
    u8 pad_38[0xEC];
    s32 unk124;
    s32 unk128;
    s32 unk12C;
    u8 pad_130[0x14];
    s32 unk144;
    f32 unk148;
    u8 pad_14C[0x4];
    s16 unk150;
    s16 unk152;
};

#define OBJ_F8(o) (*(s32 *)((u8 *)(o) + 0xF8))
extern s16 D_80052B28[];
extern void func_80005528(void *, s32);
extern f32 func_80006D00(s32, s32, s32);
extern void func_80006AE0(Vec3f *, s32, s32, f32);
extern f32 func_80006DCC(f32);

void func_80016434(struct Obj_80016434 *arg0) {
    Vec3f v;
    f32 f;
    s32 t;

    arg0->unk144 = D_80052B28[OBJ_F8(arg0)];
    func_80005528(arg0, 0);
    arg0->unk152 = arg0->unk150;
    f = func_80006D00(arg0->unk150, 0, arg0->unk144);
    arg0->unk148 = f;
    func_80006AE0(&v, arg0->unk150, 0, f);
    arg0->pos.x = v.x;
    arg0->pos.y = v.y;
    arg0->pos.z = v.z;
    t = func_80006DCC(f);
    arg0->unk124 = t;
    arg0->unk12C = t;
    arg0->unk128 = t;
}

