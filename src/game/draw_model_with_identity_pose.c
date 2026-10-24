/* Target: func_80011BE0  VRAM 0x80011BE0  ROM 0x127E0
 * Size: 0xE4  Compiler: SN GCC 2.8 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct {
    f32 x, y, z;
} Vec3f;

extern void func_8000C530(void *, unsigned int, int);
extern void func_80011930(void *pos, Vec3f *out, s32 flag);
extern void *func_8001E630(void);
extern void func_8000CF10(void *obj, f32 a, f32 b, f32 c, f32 d, f32 x, f32 y, f32 z);
extern void func_80007EB0(void *pos, Vec3f *v, void *table, void *obj, s32 flag);
extern void func_8000C674(void *pos);
extern u8 D_80059CF0[];

void func_80011BE0(void *pos, void *arg, s32 flag) {
    Vec3f a;
    Vec3f b;
    void *obj;
    f32 zero;

    func_8000C530(pos, 0x40, arg);
    zero = 0.0f;
    b.x = zero;
    a.x = zero;
    b.y = zero;
    a.y = zero;
    b.z = zero;
    a.z = zero;
    func_80011930(pos, &a, 0);
    obj = func_8001E630();
    func_8000CF10(obj, zero, zero, zero, 1.0f, a.x, a.y, a.z);
    func_80007EB0(pos, &b, D_80059CF0, obj, flag);
    func_8000C674(pos);
}
