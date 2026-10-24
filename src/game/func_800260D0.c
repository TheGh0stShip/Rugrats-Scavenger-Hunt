/* Target: func_800260D0  VRAM 0x800260D0  ROM 0x26CD0  Size: 0x14C  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Obj_800260D0 {
    u8 pad_00[0x4];
    s16 unk04;
    u8 unk06;
    u8 unk07;
    u8 pad_08;
    u8 unk09;
    u8 pad_0A[0x2];
    u32 unk0C[4];
    s32 unk1C[3];
    s32 unk28[3];
};
extern s32 D_8006FBB0[16];
extern s32 D_80052F18;
extern void *func_80038E90(void *, s32, s32);
extern s32 func_8003A840(void);

void func_800260D0(struct Obj_800260D0 *obj, s32 keep) {
    s32 i;
    s32 r;

    obj->unk06 = 30;
    obj->unk07 = 10;
    obj->unk04 = 0;
    obj->unk09 = 0;
    if (keep == 0) {
        func_80038E90(D_8006FBB0, 0, 0x40);
    }
    for (i = 0; i < 4; i++) {
        if (D_80052F18 == 0) {
            do {
                r = func_8003A840() % 4 + i * 4;
            } while (D_8006FBB0[r] != 0);
            obj->unk0C[i] = r + 0x180;
            D_8006FBB0[r] = 1;
        } else {
            obj->unk0C[i] = i + 0x190;
        }
        obj->unk0C[i] |= 0x80000000;
    }
    for (i = 2; i >= 0; i--) {
        obj->unk1C[i] = 0;
    }
    for (i = 2; i >= 0; i--) {
        obj->unk28[i] = 0;
    }
}
