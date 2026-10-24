/* func_8002B2E4  VRAM 0x8002B2E4  ROM 0x2BEE4  Size: 0x150
 * Compiler: SN Systems GCC 2.8 (-O2 -G0), asn64 with reviewed scheduling normalization
 */
typedef float f32;
typedef short s16;
typedef int s32;
typedef unsigned char u8;

typedef struct { f32 x, y, z; } Vec3f_8002B2E4;

struct Ent_800544E0 {
    Vec3f_8002B2E4 pos;   /* 0x00 */
    Vec3f_8002B2E4 posA;  /* 0x0C */
    Vec3f_8002B2E4 posB;  /* 0x18 */
    Vec3f_8002B2E4 vel;   /* 0x24 */
    s16 state;            /* 0x30 */
    u8 pad_32[2];
    s32 handle;           /* 0x34 */
    s16 counter;          /* 0x38 */
    u8 pad_3A[2];
};

extern struct Ent_800544E0 D_800544E0[2];
extern s16 D_800B92EC;
extern void *D_8005B33C;
extern f32 D_80059D48[3];
extern u8 D_80059CF0[];
extern f32 D_8000234C;
extern f32 D_80002350;
extern void *func_8001E630(void);
extern void func_80047A38(void *mtx, f32 a, f32 b, f32 c, f32 d, f32 x, f32 y, f32 z);
extern u8 *func_800197C8(int);
extern u8 *func_80019788(int);
extern void * func_8000BDFC(void *, float, float, float, void *, void *, int);
extern void func_8000C674(void *obj);

void func_8002B2E4(void) {
    s32 i;
    struct Ent_800544E0 *e;

    for (i = 0; i < 2; i++) {
        e = &D_800544E0[i];
        if (i != D_800B92EC) {
            e->counter = 0;
        }
        D_8005B33C = func_8001E630();
        func_80047A38(D_8005B33C, 0.0f, (f32)e->counter, 0.0f, 1.0f,
                      e->pos.x, e->pos.y,
                      e->pos.z + ((i == D_800B92EC) ? 800.0f : 0.0f));
        func_8000BDFC(func_800197C8(e->handle) + 0x2C, D_80059D48[0], D_80059D48[1],
                      D_80059D48[2], D_80059CF0, D_8005B33C, 0);
        func_8000C674(func_80019788(e->handle) + 0x2C);
    }
}
