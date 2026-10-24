/* func_80029314  VRAM 0x80029314  ROM 0x29F14  size 0x230
 * SN GCC 2.8 -O2 -G0 -mips3; assembled after the reviewed SN scheduling normalization. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
typedef double f64;

struct Actor_80029314 {
    u8 pad_000[0x2C];
    u8 unk2C[0xB4];
    s32 unkE0;
    u8 pad_0E4[0x34];
    u32 flags;
    u8 pad_11C[0x8];
    s32 unk124;
    u8 pad_128[0x1C];
    s32 unk144;
    f32 unk148;
    f32 unk14C;
    s16 unk150;
    u8 pad_152[0x4];
    s16 unk156;
};

struct Obj_800197C8 {
    u8 pad_00[0x2C];
    s32 unk2C;
    u8 pad_30[0xA];
    s16 unk3A;
};

extern f32 func_80007A28(struct Actor_80029314 *);
extern f32 func_80006D00(s32, s32, s32);
extern f32 func_80006D7C(s32, s32);
extern f32 func_80006DCC(f32);
extern s32 func_80005598(struct Actor_80029314 *);
extern void func_80006AE0(void *, s32, s32, f32);
extern void func_800079B0(struct Actor_80029314 *, s32);
extern void func_800069C8(s32, struct Actor_80029314 *, s32);
extern struct Obj_800197C8 *func_800197C8(s32);
extern int func_8000CE08(void *, int);
extern void func_80005500(s32, struct Actor_80029314 *);

/* Last word of the preceding unit-local jump table (func_80028F80). */
static const unsigned int jtbl_80002140_tail = 0x80029024;

void func_80029314(struct Actor_80029314 *obj) {
    f32 spd = -1.0f;
    f32 target;
    f32 x;
    s16 cur;
    struct Obj_800197C8 *p;

    if (obj->flags & 0x8000) {
        if (obj->unk156 < 0) {
            spd = 1.0f;
        }
        obj->unk148 += func_80007A28(obj) * spd;
        target = func_80006D00(obj->unk150, 0, obj->unk144);
        target += func_80006D7C(obj->unk150, 0) * spd;
        obj->unk14C = target;
        if (spd < 0.0f) {
            if (obj->unk148 <= target) {
                obj->unk148 = target;
            }
        } else if (target <= obj->unk148) {
            obj->unk148 = target;
        }
        obj->unk124 = func_80006DCC(obj->unk148);
        func_80006AE0(obj->unk2C, obj->unk150, func_80005598(obj) == 0, obj->unk148);
        if (obj->unk148 == obj->unk14C) {
            x = obj->unk148;
            if (x < 0.0f) {
                obj->unk148 = x + 360.0;
            } else if (x >= 360.0) {
                obj->unk148 = x - 360.0;
            }
            func_800079B0(obj, -spd);
            if (spd < 0.0f) {
                obj->unk156--;
            } else {
                obj->unk156++;
            }
            if (obj->unk156 == 0) {
                obj->flags &= ~0x8000;
            }
        } else {
            func_800069C8(0x22, obj, 0);
        }
    } else {
        p = func_800197C8(obj->unkE0);
        cur = p->unk3A;
        if (cur >= func_8000CE08(p->unk2C, 0x22) - 1) {
            func_80005500(6, obj);
            obj->flags |= 0x10;
            func_800069C8(0, obj, 2);
        }
    }
}
