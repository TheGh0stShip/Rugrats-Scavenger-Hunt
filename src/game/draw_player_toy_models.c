typedef unsigned char u8;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct Vec3f { f32 x, y, z; } Vec3f;
typedef struct Player_800C3DB0 {
    void *obj;     /* 0x00 */
    s32 actorId;   /* 0x04 */
    s32 unk08;     /* 0x08 */
    u8 pad_0C[0x4];
    Vec3f pos;     /* 0x10 */
    Vec3f home;    /* 0x1C */
    Vec3f vel;     /* 0x28 */
    u8 pad_34[0x4];
} Player_800C3DB0;
typedef struct Actor_800197C8 {
    u8 pad_00[0x2C];
    u8 unk2C[1]; /* 0x2C */
} Actor_800197C8;

extern Player_800C3DB0 D_800C3DB0[2];
extern s32 D_80054A9C;
extern Vec3f D_80059D48;
extern u8 D_80059CF0[];
extern void *func_8001E630(void);
extern Actor_800197C8 *func_800197C8(s32 id);
extern void func_8000C3AC(void *obj, Vec3f *out);
extern void func_80047A38(void *mtx, f32 rx, f32 ry, f32 rz, f32 scale, f32 x, f32 y, f32 z);
extern void *func_8000BDFC(void *, Vec3f, void *, void *, s32);
extern void func_8000C674(void *obj);

void func_800345EC(void) {
    s32 i;
    Player_800C3DB0 *p;
    void *mtx;
    Vec3f pos;
    Vec3f ofs;
    f32 zero;

    zero = 0.0f;
    for (i = 0; i < 2; i++) {
        p = &D_800C3DB0[i];
        mtx = func_8001E630();
        pos = p->pos;
        ofs.z = zero;
        ofs.y = zero;
        ofs.x = zero;
        func_8000C3AC(func_800197C8(p->actorId)->unk2C, &ofs);
        if (i == 1) {
            ofs.x = -ofs.x;
            ofs.y = -ofs.y;
        }
        pos.x += ofs.x;
        pos.y += ofs.y;
        pos.z += ofs.z;
        func_80047A38(mtx, zero, (f32)(i * 180), zero, 1.0f, pos.x, pos.y, pos.z);
        func_8000BDFC(func_800197C8(p->actorId)->unk2C, D_80059D48, D_80059CF0, mtx, 0x20000000);
        func_8000C674(func_800197C8(p->actorId)->unk2C);
        if (D_80054A9C != 0) {
            p->pos.x += p->vel.x;
            p->pos.y += p->vel.y;
            p->pos.z += p->vel.z;
        } else {
            p->pos = p->home;
        }
    }
    if (D_80054A9C != 0) {
        D_80054A9C--;
    }
}
