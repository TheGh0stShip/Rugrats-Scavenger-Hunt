typedef unsigned char u8;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct { u8 m[0x40]; } Mtx_8000AB74;

struct Obj_8000AB74 {
    u8 pad_00[0xC];
    s32 flags;          /* 0x0C */
    u8 pad_10[0x1C];
    f32 pos[3];         /* 0x2C */
    u8 unk38[0x28];     /* 0x38 */
    Mtx_8000AB74 mtx[2];/* 0x60 */
    u8 pad_E0[4];
    s32 unkE4;
    s32 phase;          /* 0xE8 */
    f32 vel[3];         /* 0xEC */
    f32 rot[3];         /* 0xF8 */
    f32 rotVel[3];      /* 0x104 */
    s32 unk110;
    s32 delay;          /* 0x114 */
};

extern s32 D_80054358;
extern void *D_80052A78[];
extern u8 *D_80052F04;
extern void func_8000CF10(Mtx_8000AB74 *m, f32 rx, f32 ry, f32 rz, f32 s, f32 x, f32 y, f32 z);
extern u8 *func_800197C8(int);
extern void *func_8000B910(void *ctrl, f32 *pos, void *eye, void *a3, Mtx_8000AB74 *m, u32 mask);

void func_8000AB74(struct Obj_8000AB74 *obj) {
    switch (obj->phase) {
        case 0:
            obj->rot[0] += obj->rotVel[0];
            obj->rot[1] += obj->rotVel[1];
            obj->rot[2] += obj->rotVel[2];
            obj->pos[0] += obj->vel[0];
            obj->pos[1] += obj->vel[1];
            obj->pos[2] += obj->vel[2];
            obj->vel[1] -= 25.0f;
            if (obj->pos[1] < 0.0f) {
                obj->phase = 1;
                obj->delay = 2;
            }
            func_8000CF10(&obj->mtx[D_80054358], obj->rot[0], obj->rot[1], obj->rot[2], 0.5f,
                          obj->pos[0], obj->pos[1], obj->pos[2]);
            func_8000B910(func_800197C8(D_80052A78[obj->unkE4]) + 0x2C, obj->pos, D_80052F04 + 0x28,
                          obj->unk38, &obj->mtx[D_80054358], 0);
            break;
        case 1:
            if (obj->delay != 0) {
                obj->delay--;
            } else {
                obj->flags |= 1;
            }
            break;
    }
}
