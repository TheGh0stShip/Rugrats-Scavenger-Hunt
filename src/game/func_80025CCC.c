/* Target: func_80025CCC  VRAM 0x80025CCC  ROM 0x268CC  Size: 0x94  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Obj_800055A4 {
    u8 pad_00[0x1AC];
    u32 unk1AC[4];
};
extern s32 D_80057D5C;
extern struct Obj_800055A4 *func_800055A4(s32);

s32 func_80025CCC(u32 id) {
    s32 i;
    s32 j;

    for (i = 0; i < D_80057D5C; i++) {
        for (j = 0; j < 4; j++) {
            if (func_800055A4(i)->unk1AC[j] == (id | 0x80000000)) {
                return 1;
            }
        }
    }
    return 0;
}
