/* Target: func_8002621C  VRAM 0x8002621C  ROM 0x26E1C  Size: 0x78  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Obj_8002621C {
    u8 pad_00[0x1A7];
    s8 unk1A7;
};
extern int func_8001F138(s32);

void func_8002621C(struct Obj_8002621C *obj, s32 delta) {
    s32 v;

    if (delta > 0) {
        func_8001F138(4);
    } else if (delta < 0) {
        func_8001F138(0x20);
    }
    v = obj->unk1A7 + delta;
    if (v < 0) {
        obj->unk1A7 = 0;
    } else if (v >= 100) {
        obj->unk1A7 = 99;
    } else {
        obj->unk1A7 += delta;
    }
}
