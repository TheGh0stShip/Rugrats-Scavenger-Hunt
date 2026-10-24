/* Target: func_80026294  VRAM 0x80026294  ROM 0x26E94  Size: 0x78  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Obj_80026294 {
    u8 pad_00[0x1A6];
    s8 unk1A6;
};
extern int func_8001F138(s32);

void func_80026294(struct Obj_80026294 *obj, s32 delta) {
    s32 v;

    if (delta > 0) {
        func_8001F138(0x1D);
    } else if (delta < 0) {
        func_8001F138(0x20);
    }
    v = obj->unk1A6 + delta;
    if (v < 0) {
        obj->unk1A6 = 0;
    } else if (v >= 31) {
        obj->unk1A6 = 30;
    } else {
        obj->unk1A6 += delta;
    }
}
