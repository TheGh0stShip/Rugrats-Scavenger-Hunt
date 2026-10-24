/* Target: func_800094A4  VRAM 0x800094A4  ROM 0xA0A4  Size: 0xB0
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef unsigned char u8;
typedef signed char s8;
typedef int s32;
struct Obj_800094A4 {
    u8 pad_000[0x134];
    s8 unk134[8];
};

extern s32 D_80052F18;
extern s32 D_800529F8;
extern s32 D_80057D5C;
extern void *func_800055A4(s32 index);
extern void func_80009554(void *a, void *b);

s32 func_800094A4(struct Obj_800094A4 *obj) {
    s32 result = 0;
    s32 count = 0;
    s32 i;
    s32 last;

    if (D_80052F18 == 1 && D_800529F8 != 0) {
        return result;
    }
    for (i = 0; i < D_80057D5C; i++) {
        if (obj->unk134[i] != -1) {
            last = obj->unk134[i];
            count++;
        }
    }
    if (count == 1) {
        func_80009554(obj, func_800055A4(last));
        result = 1;
    }
    return result;
}
