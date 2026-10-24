/* func_80029BB4  VRAM 0x80029BB4  ROM 0x2A7B4  size 0x1B4
 * SN GCC 2.8 -O2 -G0 -mips3; assembled after the reviewed SN scheduling normalization. */
typedef unsigned char u8;
typedef signed char s8;
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

extern s32 D_80054378;
extern s32 D_80054370;
extern f32 D_80059D48[3];
extern u8 D_80059CF0[];
extern void *func_8001E630(void);
extern void func_8000CF10(void *, float, float, float, float, float, float, float);
extern u8 *func_800197C8(s32);
extern void * func_8000BDFC(void *, float, float, float, void *, void *, int);
extern void func_80006E30(void *);
extern int func_8001F138(s32);
extern void func_8001A5C0(void);

extern void func_80029D68(struct Actor_800055A4 *);
extern s32 func_80029DC4(struct Actor_800055A4 *, u32, s32);

s32 func_80029BB4(struct Actor_800055A4 *obj, u32 input) {
    void *h;
    s32 ret;
    s32 v;

    h = func_8001E630();
    D_80054370 = (D_80054370 + 10) % 360;
    func_8000CF10(h, 0.0f, (f32)D_80054370, 0.0f, 1.0f, 0.0f, 0.0f, -4000.0f);
    func_8000BDFC(func_800197C8(0x1F7) + 0x2C, D_80059D48[0], D_80059D48[1], D_80059D48[2],
                  D_80059CF0, h, 0);
    ret = 0;
    switch (D_80054378) {
        case 0:
            func_80029D68(obj);
            if (input & 0x10) {
                D_80054378++;
            } else if (input & 0x40) {
                ret = 1;
                func_8001A5C0();
            }
            break;
        case 1:
            v = func_80029DC4(obj, input, 0);
            if (v != 0) {
                obj->unk154 = v;
                obj->flags |= 0x800;
                func_80006E30(obj);
                ret = 1;
                func_8001F138(0x46);
            }
            break;
    }
    if (ret != 0) {
        D_80054378 = 0;
    }
    return ret;
}
