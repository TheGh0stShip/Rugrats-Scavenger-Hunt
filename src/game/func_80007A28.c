/* Target: func_80007A28  VRAM 0x80007A28  ROM 0x8628
 * Size: 0xD8  Pipeline: SN GCC 2.8 -O2 -G0 (normalized return), .rdata at 0x800005D0
 * Confidence: high */
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef float f32;

struct Res_80007A28 {
    u8 pad_00[0x2C];
    s32 unk2C;
};
struct Obj_80007A28 {
    u8 pad_000[0xE0];
    s32 unkE0;
    u8 pad_0E4[0x6C];
    s16 unk150;
};

extern s32 D_80052F18;
extern struct Res_80007A28 *func_800197C8(s32 arg0);
extern int func_8000CDC8(void *, int);
extern s32 func_80005598(struct Obj_80007A28 *obj);
extern f32 func_80006D7C(s32 id, s32 arg1);

f32 func_80007A28(struct Obj_80007A28 *obj) {
    s32 n;
    f32 f;

    n = func_8000CDC8(func_800197C8(obj->unkE0)->unk2C, 1);
    n += n & 1;
    if (n != 8 && n != 10 && n != 12 && n != 16 && n != 20 && n != 24) {
        n = 10;
    }
    if (D_80052F18 == 2) {
        f = func_80006D7C(obj->unk150, func_80005598(obj) == 0) / (f32)n;
    } else {
        f = 600.0f / (f32)n;
    }
    if (n >= 13) {
        f += f;
    }
    return f;
}
