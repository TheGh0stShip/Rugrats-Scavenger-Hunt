/* func_800296D4  VRAM 0x800296D4  ROM 0x2A2D4  size 0x1A8
 * SN GCC 2.8 -O2 -G0 -mips3; assembled after the reviewed SN scheduling normalization. */
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Actor_800055A4 {
    u8 pad_000[0x118];
    u32 flags;
    u8 pad_11C[0x38];
    s16 unk154;
};

struct Obj_800B9290 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
};

extern struct Obj_800B9290 D_800B9290;
extern s32 D_80054370;
extern f32 D_80059D48[3];
extern u8 D_80059CF0[];
extern void *func_8001E630(void);
extern void func_8000CF10(void *, f32, f32, f32, f32, f32, f32, f32);
extern u8 *func_800197C8(s32);
extern void *func_8000BDFC(void *, f32, f32, f32, void *, void *, s32);
extern void func_80006E30(void *);
extern s32 func_8001F138(s32);
extern void func_8001A5C0(void);
extern s32 func_8002987C(struct Actor_800055A4 *, s32, u32);
extern void func_8002E6D0(struct Actor_800055A4 *, s32, s32);

s32 func_800296D4(struct Actor_800055A4 *obj, u32 input) {
    struct Obj_800B9290 *st = &D_800B9290;
    s16 ret = 0;
    s32 id;
    void *h;
    s32 v;

    id = st->unk4 * 2 + 0x1EF;
    h = func_8001E630();
    D_80054370 = (D_80054370 + 10) % 360;
    func_8000CF10(h, 0.0f, (f32)D_80054370, 0.0f, 1.0f, 0.0f, 0.0f, -4000.0f);
    func_8000BDFC(func_800197C8(id) + 0x2C, D_80059D48[0], D_80059D48[1], D_80059D48[2],
                  D_80059CF0, h, 0);
    v = func_8002987C(obj, st->unk4, input);
    if (v == 2) {
        obj->flags |= 0x800;
        obj->unk154 = ((u16 *)&st->unk4)[1] + 1;
        func_80006E30(obj);
        func_8001F138(0x46);
        func_8002E6D0(obj, 8, -5);
        ret = 1;
    } else if (v == 1) {
        func_8001A5C0();
        ret = 1;
    }
    return ret;
}
