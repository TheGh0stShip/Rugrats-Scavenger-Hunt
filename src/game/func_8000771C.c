/* Target: func_8000771C  VRAM 0x8000771C  ROM 0x831C
 * Size: 0x270  Pipeline: SN GCC 2.8 -O2 -G0 (reviewed truncation-register normalization), .rdata at 0x800005C8
 * Confidence: medium */
typedef unsigned char u8;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;
typedef double f64;

struct Anim_8000771C {
    u8 pad_00[0x8];
    s16 unk8;
    s16 unkA;
};
struct Res_8000771C {
    u8 pad_00[0x2C];
    struct Anim_8000771C anim;
};
struct Obj_8000771C {
    u8 pad_000[0x2C];
    u8 unk2C[0xB4];
    s32 unkE0;
    u8 pad_0E4[0x18];
    s32 unkFC;
    u8 pad_100[0x18];
    s32 unk118;
    u8 pad_11C[0x8];
    s32 unk124;
    u8 pad_128[0x20];
    f32 unk148;
    f32 unk14C;
    s16 unk150;
};
struct Global_80052F04 {
    u8 pad_000[0xF0];
    s32 unkF0;
};

extern s32 D_800529F8;
extern s32 D_800535E4;
extern struct Global_80052F04 *D_80052F04;

extern struct Res_8000771C *func_800197C8(s32 arg0);
extern void func_80005500(s32 arg0, struct Obj_8000771C *arg1);
extern f32 func_80007A28(struct Obj_8000771C *arg0);
extern f32 func_80006DCC(f32 arg0);
extern s32 func_80005598(struct Obj_8000771C *arg0);
extern void func_80006AE0(void *, int, int, float);
extern void func_800079B0(struct Obj_8000771C *arg0, s32 arg1);
extern void func_800069C8(s32 arg0, struct Obj_8000771C *arg1, s32 arg2);
extern void func_8002BD44(struct Obj_8000771C *arg0, s32 arg1);

void func_8000771C(struct Obj_8000771C *obj) {
    struct Anim_8000771C *anim;
    s32 flag = 0;
    f32 f;
    s16 v;

    anim = &func_800197C8(obj->unkE0)->anim;
    if (obj->unkFC == 1) {
        if (D_800529F8 == 0) {
            flag = 1;
            func_80005500(6, obj);
            goto tail;
        }
        obj->unk118 &= ~0x400;
        f = obj->unk148 - func_80007A28(obj);
        obj->unk148 = f;
        if (f <= obj->unk14C) {
            obj->unk148 = obj->unk14C;
        }
        obj->unk124 = func_80006DCC(obj->unk148);
        func_80006AE0(obj->unk2C, obj->unk150, func_80005598(obj) == 0, obj->unk148);
        if (obj->unk148 == obj->unk14C) {
            if (obj->unk148 < 0.0f) {
                obj->unk148 += 360.0;
            }
            if (D_800535E4 == 0) {
                D_800529F8--;
            }
            func_80005500(6, obj);
            func_800079B0(obj, 1);
        } else {
            v = anim->unk8;
            if (v == 5 || v == 13) {
                func_800069C8(1, obj, 2);
            } else if (v != 0 && v != 8) {
                func_800069C8(1, obj, 2);
            } else {
                func_800069C8(5, obj, 0);
                if (anim->unkA == 0) {
                    func_800069C8(1, obj, 0);
                }
            }
        }
    } else if ((u32)(obj->unkFC - 2) >= 2) {
        flag = 1;
    }
tail:
    if (flag) {
        v = anim->unk8;
        if (v == 6 || v == 14 || (v != 1 && v != 9)) {
            func_800069C8(0, obj, 2);
        } else {
            func_800069C8(6, obj, 2);
            if (anim->unkA == 0) {
                func_800069C8(0, obj, 0);
            }
        }
    }
    if (D_80052F04->unkF0 == 0 && obj->unkFC != 10) {
        func_8002BD44(obj, D_800529F8);
    }
}
