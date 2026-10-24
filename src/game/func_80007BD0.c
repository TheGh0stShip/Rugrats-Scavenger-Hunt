/* Target: func_80007BD0  VRAM 0x80007BD0  ROM 0x87D0
 * Size: 0x130  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef unsigned char u8;
typedef int s32;
typedef float f32;

struct Res_80007BD0 {
    u8 pad_00[0x2C];
    s32 unk2C;
};
struct Obj_80007BD0 {
    u8 pad_000[0x2C];
    f32 pos[3];
    u8 unk38[0x28];
    f32 mtx[2][4][4];
    s32 unkE0;
    u8 pad_0E4[0x10];
    s32 unkF4;
    s32 unkF8;
    u8 pad_0FC[0x1C];
    s32 unk118;
};

extern s32 D_80052EE4;
extern s32 D_80057D5C;
extern s32 D_80052FFC[];
extern s32 D_8005300C[];
extern s32 D_80054358;

extern struct Res_80007BD0 *func_800197C8(s32 arg0);
extern void func_80019BD4(s32 arg0);
extern void func_80007EB0(void *, void *, void *, void *, int);
extern void func_8000C674(s32 *arg0);

void func_80007BD0(struct Obj_80007BD0 *obj) {
    s32 *res;
    s32 flags = 0;

    res = &func_800197C8(obj->unkE0)->unk2C;
    if (D_80052EE4 != 0) {
        flags = (obj->unkF4 < D_80057D5C) << 20;
    }
    if (obj->unk118 & 0x60) {
        if (obj->unk118 & 0x20) {
            res = &func_800197C8(D_80052FFC[obj->unkF8])->unk2C;
        } else {
            func_80019BD4(D_80052FFC[obj->unkF8]);
        }
        obj->unk118 &= ~0x40;
    }
    if (obj->unk118 & 0x10000) {
        res = &func_800197C8(D_8005300C[obj->unkF8])->unk2C;
    }
    func_80007EB0(res, obj->pos, obj->unk38, obj->mtx[D_80054358], flags);
    func_8000C674(res);
}
