typedef unsigned char u8;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Actor_8000B00C {
    u8 pad_00[0x2C];
    f32 pos[3];     /* 0x2C */
    u8 pad_38[0xA8];
    s32 unkE0;
    u8 pad_E4[0x34];
    u32 flags;      /* 0x118 */
    u8 pad_11C[8];
    s32 angle;      /* 0x124 */
    u8 pad_128[0x1C];
    s32 dir;        /* 0x144 */
    f32 facing;     /* 0x148 */
};

extern u32 D_80057D5C;
extern u32 D_80057D58;
extern s32 D_800581A4;
extern struct Actor_8000B00C *func_8000AD88(void);
extern u32 func_8001BF3C(void);
extern struct Actor_8000B00C *func_800206D0(void);
extern struct Actor_8000B00C *func_800055A4(s32 idx);
extern void func_8000DDAC(s32 a);
extern void func_80005528(struct Actor_8000B00C *a, s32 b);
extern f32 func_80006D00(s32 x, s32 y, s32 value);
extern f32 func_80006DCC(f32 angle);
extern void func_80006AE0(void *object, s32 x, s32 y, f32 scale);
extern void func_8000FDAC(s32 a, struct Actor_8000B00C *b);
extern void func_80012D9C(s32 a);
extern void func_80012D7C(s32 a);
extern void func_8001BF24(s32 a);
extern s32 func_80012CE8(void);
extern u8 *func_800197C8(s32 arg);
extern void func_8000C530(void *ctrl, s32 idx, s32 mode);
extern s32 func_8000CE08(void *model, s32 idx);

s32 func_8000B00C(void) {
    s32 ret = 0;
    struct Actor_8000B00C *self;
    struct Actor_8000B00C *other;
    u32 i;

    self = func_8000AD88();
    switch (func_8001BF3C()) {
        case 1:
            other = func_800206D0();
            for (i = 0; i < D_80057D5C + D_80057D58; i++) {
                if (func_800055A4(i) != self && func_800055A4(i) != other) {
                    func_800055A4(i)->flags |= 0x100000;
                }
            }
            self->flags |= 0x400000;
            func_8000DDAC(0);
            func_80005528(self, 0);
            other->dir = 0;
            other->facing = func_80006D00(0, 1, 0);
            other->angle = func_80006DCC(other->facing - 90.0f);
            func_80006AE0(other->pos, 0, 1, other->facing);
            self->facing = func_80006D00(0, 1, self->dir = 4);
            self->angle = func_80006DCC(self->facing - 90.0f);
            func_80006AE0(self->pos, 0, 1, self->facing);
            func_8000FDAC(9, self);
            func_80012D9C(10);
            func_8001BF24(3);
            break;
        case 3:
            if (func_80012CE8() == 1) {
                func_8000C530(func_800197C8(self->unkE0) + 0x2C, 0xA, 1);
                func_8000C530(func_800197C8(self->unkE0) + 0x2C, 0, 2);
                D_800581A4 = func_8000CE08(*(void **)(func_800197C8(self->unkE0) + 0x2C), 0xA);
                func_8001BF24(4);
            }
            break;
        case 4:
            if (--D_800581A4 == 0) {
                func_80012D7C(0x14);
                func_8001BF24(5);
            }
            break;
        case 5:
            if (func_80012CE8() == 1) {
                ret = 1;
            }
            break;
    }
    return ret;
}
