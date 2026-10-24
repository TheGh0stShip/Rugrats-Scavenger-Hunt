typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;
typedef float f32;

typedef struct { f32 x, y, z; } Vec3f;

struct DebrisObject {
    u8 pad_00[8];
    s16 unk08;
    s16 unk0A;
    s32 unk0C;
    void *update;
    u8 pad_14[4];
    void *destroy;
    u8 pad_1C[0x10];
    Vec3f pos;
    u8 pad_38[0xA8];
    s32 unkE0;
    s32 kind;
    s32 phase;
    Vec3f vel;
    u8 pad_F8[0xC];
    Vec3f rotVel;
    u8 pad_110[8];
};

extern struct DebrisObject *D_800581A8;
extern void *D_80059DA8;
extern void *func_8001A2B0(s32 size);
extern void *func_80038E90(void *dst, s32 val, s32 size);
extern s32 func_8003A840(void);
extern void func_80007EFC(void *, void *, int, void *);
extern void func_8000A9E0(void);
extern void func_8000AD14(void *);

void func_8000A750(s32 kind)
{
    struct DebrisObject *obj;
    struct DebrisObject *src;
    s32 one = 1;

    obj = func_8001A2B0(0x118);
    func_80038E90(obj, 0, 0x118);
    obj->update = func_8000A9E0;
    obj->destroy = func_8000AD14;
    obj->unk08 = 8;
    obj->unk0C = 2;
    obj->unkE0 = one;
    obj->unk0A = one;
    src = D_800581A8;
    obj->kind = kind;
    obj->phase = 0;
    obj->pos = src->pos;
    obj->vel = src->vel;
    obj->pos.x -= obj->vel.x * 0.2;
    obj->pos.z -= obj->vel.z * 0.2;
    obj->vel.x += func_8003A840() % 40 - 20;
    obj->vel.y += func_8003A840() % 20 - 10;
    obj->vel.z += func_8003A840() % 40 - 20;
    obj->rotVel.x = func_8003A840() % 40 - 20;
    obj->rotVel.y = func_8003A840() % 40 - 20;
    obj->rotVel.z = func_8003A840() % 40 - 20;
    func_80007EFC(obj, D_80059DA8, 4, 0);
}
