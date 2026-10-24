/* Target: func_80005654  VRAM 0x80005654  ROM 0x6254
 * Size: 0x144  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef unsigned char u8;
typedef int s32;

typedef struct Actor_80005654 {
    /* 0x00 */ u8 pad_00[0xE0];
    /* 0xE0 */ s32 unk_E0;
} Actor_80005654;

extern s32 D_800535E0;
extern s32 D_80052F18;
extern s32 D_80057D58;
extern s32 D_80057D5C;

extern s32 func_8001BF00(void);
extern void func_8001BED8(s32 a);
extern Actor_80005654 *func_800055A4(s32 i);
extern s32 func_80019740(s32 a);
extern u8 *func_800197C8(s32 a);
extern void func_8000C530(void *a, s32 b, s32 c);
extern void func_80005500(s32 a, Actor_80005654 *p);

void func_80005654(void) {
    s32 i;

    if (D_800535E0 == 0) {
        return;
    }
    if (func_8001BF00() == 0x31) {
        return;
    }
    if (func_8001BF00() == 0x32) {
        return;
    }
    for (i = 0; i < D_80057D5C + D_80057D58; i++) {
        if (!(i < D_80057D5C) || func_80019740(func_800055A4(i)->unk_E0) != 0 ||
            !(i < D_80057D5C) || D_80052F18 == 2) {
            func_8000C530(func_800197C8(func_800055A4(i)->unk_E0) + 0x2C, 0, 0);
        }
        func_80005500(0, func_800055A4(i));
    }
    func_8001BED8(0x31);
}
