/* Target: func_80011A40  VRAM 0x80011A40  ROM 0x12640
 * Size: 0x1A0  Compiler: SN GCC 2.8 (-O2)
 * Confidence: low
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

struct Obj_80005544 {
    u8 pad_00[0xF8];
    u32 unk_F8;
};

extern struct Obj_80005544 *func_80005544(void);
extern u8 *func_800197C8(s32 id);
extern void func_80011930(void *pos, Vec3f *out, s32 flag);
extern void *func_8001E630(void);
extern void func_8000CF10(void *obj, f32 a, f32 b, f32 c, f32 d, f32 x, f32 y, f32 z);
extern void func_8000C530(void *, unsigned int, int);
extern void func_80007EB0(void *pos, Vec3f *v, f32 *out, void *obj, s32 flag);
extern void func_8000C674(void *pos);
extern s32 D_8005300C[];
extern s32 D_80058BE0;

void func_80011A40(s32 arg) {
    struct Obj_80005544 *player = func_80005544();
    u8 i;
    f32 far;
    f32 zero;

    far = 1800.0f;
    zero = 0.0f;
    for (i = 0; i < 2; i++) {
        Vec3f a;
        Vec3f b;
        f32 v[4];
        u8 *pos;
        void *obj;

        if (i == 0) {
            pos = func_800197C8(D_8005300C[player->unk_F8]) + 0x2C;
            v[1] = 0.0f;
        } else {
            if (player->unk_F8 < 2) {
                D_80058BE0 = 0x87;
            } else {
                D_80058BE0 = 0x88;
            }
            pos = func_800197C8(D_80058BE0) + 0x2C;
            v[1] = 180.0f;
        }
        b.x = far;
        a.x = far;
        b.y = zero;
        a.y = zero;
        b.z = far;
        a.z = far;
        func_80011930(pos, &a, (s32)v[1]);
        obj = func_8001E630();
        func_8000CF10(obj, zero, v[1], zero, 1.0f, a.x, a.y, a.z);
        func_8000C530(pos, 0x3F, arg);
        func_80007EB0(pos, &b, v, obj, 0);
        func_8000C674(pos);
    }
}
