/* Target: func_800166B0  VRAM 0x800166B0  ROM 0x172B0
 * Size: 0x40  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */

typedef unsigned char u8;
typedef signed int s32;

struct Obj_800166B0 {
    u8 pad_00[0x11C];
    s32 unk11C;
    u8 pad_120[0x6C];
    s32 unk18C;
};

extern s32 func_80016530(s32);

s32 func_800166B0(struct Obj_800166B0 *arg0) {
    if (arg0->unk11C == -1) {
        return arg0->unk18C;
    }
    return func_80016530(arg0->unk11C);
}
