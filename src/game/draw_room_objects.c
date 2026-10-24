/* func_800319B8: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Vec3f;
struct Entry_800319B8 {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    u8 pad_0A[0x2];
    s32 unkC;
};
struct Obj_800319B8 {
    u8 pad_00[0xA];
    s16 count;
    u8 pad_0C[0x4];
    struct Entry_800319B8 entries[1];
};
extern u8 D_800581E9;
extern void *D_800529F0;
extern u8 *D_80052F04;
extern u8 D_80059CF0[];
extern struct Obj_800319B8 *func_8000DD88(s32);
extern s32 func_80025FC4(void *, s32);
extern void *func_8001E630(void);
extern void func_8000CF10(void *, float, float, float, float, float, float, float);
extern u8 *func_800197C8(s32);
extern void * func_8000B910(void *, void *, void *, void *, void *, int);
extern void func_8000C674(void *);

void func_800319B8(s32 arg0) {
    struct Obj_800319B8 *obj;
    struct Entry_800319B8 *e;
    s32 flag;
    s32 i;
    void *h;
    u8 *p;
    f32 ang;
    Vec3f pos;

    flag = 0;
    obj = func_8000DD88(D_800581E9);
    if (D_800529F0 != 0) {
        flag = func_80025FC4(D_800529F0, 0xF) != 0;
    }
    for (i = 0; i < obj->count; i++) {
        e = &obj->entries[i];
        if ((u16)(e->unk4 - 0x106) < 4) {
            if (!flag) {
                continue;
            }
        } else if ((u16)(e->unk4 - 0x10A) < 0xC) {
            if (flag) {
                continue;
            }
        }
        pos.x = e->unk2 * 600 + 300;
        pos.y = 0.0f;
        pos.z = e->unk0 * 600 + 300;
        if (e->unk6 == 0x5A) {
            ang = 270.0f;
        } else if (e->unk6 == 0x10E) {
            ang = 90.0f;
        } else {
            ang = e->unk6;
        }
        if ((u16)(e->unk4 - 0xFE) < 2) {
            pos.x -= 300.0f;
            pos.z -= 300.0f;
        }
        h = func_8001E630();
        func_8000CF10(h, 0.0f, ang, 0.0f, 1.0f, pos.x, pos.y, pos.z);
        p = func_800197C8(e->unk4) + 0x2C;
        if (e->unk4 == 0x137) {
            func_8000B910(p, &pos, &pos, D_80059CF0, h, obj->entries[i].unkC);
        } else {
            func_8000B910(p, &pos, D_80052F04 + 0x28, D_80059CF0, h, obj->entries[i].unkC);
        }
        func_8000C674(p);
    }
}
