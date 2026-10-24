/* Target: func_800120D0  VRAM 0x800120D0  ROM 0x12CD0
 * Size: 0xFC  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Anim_800197C8 {
    u8 pad_00[0x2C];
    s32 unk2C;
    u8 pad_30[0x0A];
    s16 unk3A;
};

struct Obj_800120D0 {
    u8 pad_00[0xF8];
    s32 unkF8;
    u8 pad_FC[0x118 - 0xFC];
    u32 unk118;
    u8 pad_11C[0x124 - 0x11C];
    s32 unk124;
    s32 unk128;
};

extern s32 D_8005300C[];

extern void func_80011CC4(void *);
extern struct Anim_800197C8 *func_800197C8(s32);
extern int func_8000CE08(void *, int);
extern void func_80019BD4(s32);
extern void func_80005500(s32, struct Obj_800120D0 *);
extern void func_800069C8(s32, struct Obj_800120D0 *, s32);

void func_800120D0(struct Obj_800120D0 *obj) {
    struct Anim_800197C8 *a;
    s32 frame;

    if (!(obj->unk118 & 0x10000)) {
        func_80011CC4(0);
        a = func_800197C8(D_8005300C[obj->unkF8]);
        frame = a->unk3A;
        if (frame >= func_8000CE08(a->unk2C, 0x40) - 1) {
            obj->unk118 |= 0x10000;
            func_80019BD4(D_8005300C[obj->unkF8]);
        }
    } else {
        func_80005500(6, obj);
        obj->unk124 = obj->unk128 = 0x10E;
        obj->unk118 &= ~0x100000;
        func_800069C8(0, obj, 0);
    }
}
