/* Target: func_800059EC  VRAM 0x800059EC  ROM 0x65EC
 * Size: 0x110  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef unsigned char u8;
typedef int s32;

typedef struct Actor_800059EC {
    /* 0x00 */ u8 pad_00[0xF8];
    /* 0xF8 */ s32 unk_F8;
} Actor_800059EC;

extern s32 D_80052F18;
extern s32 D_800535E0;
extern s32 D_80057D5C;

extern Actor_800059EC *func_800055A4(s32 i);
extern s32 func_800301C8(Actor_800059EC *p, s32 slot);
extern s32 func_80025E48(void);
extern void func_80012D04(s32 a, s32 b);

s32 func_800059EC(void) {
    s32 ret = 0;
    s32 i;
    Actor_800059EC *p;

    if (D_80052F18 == 2) {
        for (i = ret; i < D_80057D5C; i++) {
            p = func_800055A4(i);
            if (func_800301C8(p, 0) && func_800301C8(p, 1) && func_800301C8(p, 2) &&
                func_800301C8(p, 3)) {
                ret = 1;
                D_800535E0 = p->unk_F8 + 2;
            }
        }
    } else if (func_80025E48() != 0) {
        ret = 1;
    }
    if (ret) {
        func_80012D04(20, 0);
    }
    return ret;
}
