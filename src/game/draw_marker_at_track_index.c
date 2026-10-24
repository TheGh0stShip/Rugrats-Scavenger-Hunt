/* func_8002B044  VRAM 0x8002B044  ROM 0x2BC44  size 0x134
 * SN GCC 2.8 -O2 -G0 -mips3; assembled after the reviewed SN scheduling normalization. */
typedef float f32;
typedef int s32;
typedef unsigned char u8;

typedef struct { f32 x, y, z; } Vec3f_8002B044;

extern void *D_8005B33C;
extern u8 *D_80052F04;
extern u8 D_80059CF0[];
extern s32 func_80006DB0(s32 a, s32 b);
extern f32 func_80006D00(s32 a, s32 b, s32 c);
extern void *func_8002A9A4(s32 a, s32 b);
extern void *func_8001E630(void);
extern void func_80047A38(void *mtx, f32 a, f32 b, f32 c, f32 d, f32 x, f32 y, f32 z);
extern void * func_8000B910(void *, void *, void *, void *, void *, int);

void func_8002B044(s32 arg0, s32 arg1, s32 arg2) {
    s32 n;
    f32 angle;
    void *obj;
    Vec3f_8002B044 v;

    n = func_80006DB0(arg2, 0);
    if (arg1 >= n) {
        arg1 -= n;
    } else if (arg1 < 0) {
        arg1 += n;
    }
    angle = func_80006D00(arg2, 0, arg1);
    obj = func_8002A9A4(arg0, arg2);
    v.x = v.z = 0.0f;
    if (arg2 > 0) {
        v.y = 20.0f;
    } else {
        v.y = 10.0f;
    }
    D_8005B33C = func_8001E630();
    func_80047A38(D_8005B33C, 0.0f, angle, 0.0f, 1.0f, v.x, v.y, v.z);
    func_8000B910(obj, &v, D_80052F04 + 0x28, D_80059CF0, D_8005B33C, 0);
}
