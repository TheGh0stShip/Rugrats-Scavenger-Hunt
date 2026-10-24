/* Target: func_800112D0  VRAM 0x800112D0  ROM 0x11ED0
 * Size: 0x130  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Obj_800112D0 {
    u8 pad_00[8];
    s16 unk_08;
    s16 unk_0A;
    s32 unk_0C;
    void (*unk_10)(struct Obj_800112D0 *);
    u8 pad_14[4];
    void (*unk_18)(struct Obj_800112D0 *);
    u8 pad_1C[0x2C - 0x1C];
    f32 unk_2C;
    f32 unk_30;
    f32 unk_34;
    u8 pad_38[0xA0 - 0x38];
    void *unk_A0;
    u8 pad_A4[0xC4 - 0xA4];
    s32 unk_C4;
    s32 unk_C8;
    u8 pad_CC[0xD4 - 0xCC];
    s32 unk_D4;
    s32 unk_D8;
    s32 unk_DC;
    s32 unk_E0;
};

extern struct Obj_800112D0 *func_8001A2B0(s32 size);
extern void *func_80038E90(void *dst, s32 value, s32 size);
extern void *func_800197C8(s32 id);
extern void func_80011400(struct Obj_800112D0 *obj);
extern void func_800114CC(struct Obj_800112D0 *obj);
extern void func_80007EFC(void *, void *, int, void *);
extern void *D_80059DA8;

struct Obj_800112D0 *func_800112D0(s32 id, s32 x, s32 y, s32 z, s32 dir, s32 life) {
    struct Obj_800112D0 *o = func_8001A2B0(0x200);

    func_80038E90(o, 0, 0xE8);
    o->unk_A0 = func_800197C8(id);
    o->unk_10 = func_80011400;
    o->unk_18 = func_800114CC;
    o->unk_08 = 7;
    o->unk_0C = 2;
    o->unk_0A = dir;
    if (life != 0) {
        o->unk_C4 = 1;
        o->unk_C8 = life;
    } else {
        o->unk_C4 = 3;
        o->unk_C8 = 20;
    }
    o->unk_2C = x;
    o->unk_30 = y;
    o->unk_34 = z;
    o->unk_D4 = 0;
    o->unk_DC = 0;
    o->unk_D8 = 0;
    o->unk_E0 = 0;
    func_80007EFC(o, D_80059DA8, 4, 0);
    return o;
}
