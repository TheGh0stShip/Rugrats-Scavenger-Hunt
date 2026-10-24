typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef float f32;
typedef struct { f32 x, y, z; } Vec3f;

struct Src_8000A63C {
    u8 pad_00[0x2C];
    Vec3f pos;
};

struct Obj_8000A63C {
    u8 pad_00[8];
    s16 unk08;
    s16 unk0A;
    s32 unk0C;
    void *update;          /* 0x10 */
    u8 pad_14[4];
    void *destroy;         /* 0x18 */
    u8 pad_1C[0x14];
    f32 unk30;
    u8 pad_34[0xAC];
    s32 unkE0;
    u8 pad_E4[4];
    s32 unkE8;
    Vec3f pos;             /* 0xEC */
    u8 pad_F8[0x18];
    s32 unk110;
    u8 pad_114[4];
};

extern struct Obj_8000A63C *D_800581A8;
extern void *D_80059DA8;
extern void *func_8001A2B0(s32 size);
extern void *func_80038E90(void *dst, s32 val, s32 size);
extern void func_80007EFC(void *, void *, int, void *);
extern void func_8000A9E0(void);
extern void func_8000AD14(void *);

void func_8000A63C(struct Src_8000A63C *src) {
    struct Obj_8000A63C *obj;

    if (D_800581A8 == 0) {
        obj = func_8001A2B0(0x118);
        func_80038E90(obj, 0, 0x118);
        obj->update = func_8000A9E0;
        obj->destroy = func_8000AD14;
        obj->unk08 = 8;
        obj->unkE0 = 0;
        obj->unkE8 = 0;
        obj->unk0A = 0;
        obj->unk0C = 2;
        obj->unk30 = 600.0f;
        obj->pos = src->pos;
        obj->pos.x = obj->pos.x * 0.03260869565217391;
        obj->pos.z = obj->pos.z * 0.03260869565217391;
        obj->unk110 = 0x14;
        obj->pos.y = 350.0f;
        func_80007EFC(obj, D_80059DA8, 4, 0);
        D_800581A8 = obj;
    }
}
