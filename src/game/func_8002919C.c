/* Target: func_8002919C  VRAM 0x8002919C  ROM 0x29D9C
 * Size: 0x134  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Unresolved: func_80006AE0 4th arg is a float passed in a3 (prototype assumed) */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Actor_800055A4 {
    u8 pad_000[0x2C];
    u8 unk2C[0xEC];
    u32 flags;
    u8 pad_11C[0x8];
    s32 unk124;
    s32 unk128;
    u8 pad_12C[0x18];
    s32 unk144;
    f32 unk148;
};

extern s32 func_8000E154(s32, s32);
extern f32 func_80006D00(s32, s32, s32);
extern f32 func_80006DCC(f32);
extern void func_80006AE0(void *, s32, s32, f32);
extern void func_80005500(s32, struct Actor_800055A4 *);

void func_8002919C(struct Actor_800055A4 *obj) {
    s32 i = 0;
    s32 idx = (obj->unk144 + 1) % 24;
    f32 f;

    do {
        if ((s16)func_8000E154(0, idx) == 11) {
            break;
        }
        i++;
        idx = (idx + 1) % 24;
    } while (i < 12);

    obj->unk144 = idx;
    obj->unk148 = func_80006D00(0, 0, idx);
    f = func_80006DCC(obj->unk148);
    obj->unk124 = obj->unk128 = (s32)f;
    func_80006AE0(obj->unk2C, 0, 0, obj->unk148);
    obj->flags |= 0x10;
    func_80005500(6, obj);
}
