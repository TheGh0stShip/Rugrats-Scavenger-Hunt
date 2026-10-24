/* Target: func_8001E204  VRAM 0x8001E204  ROM 0x1EE04  Size: 136
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct {
    u32 unk0;
    u32 unk4;
} Range_8001E204;

typedef struct {
    s32 unk0;
    Range_8001E204 *unk4;
    u8 pad_08[0xA8];
    s32 unkB0;
} Block_8005B830;

extern Block_8005B830 D_8005B830;
extern Range_8001E204 *func_80013238(void);
extern void func_8000FAE8(Range_8001E204 *);

void func_8001E204(s32 arg0) {
    Range_8001E204 *r;

    r = func_80013238();
    D_8005B830.unk4 = r;
    func_8000FAE8(r);
    D_8005B830.unk0 = arg0;
    if (arg0 == 0xB9) {
        D_8005B830.unk4->unk0 -= 0x1C;
        r = D_8005B830.unk4;
        if (r->unk0 < r->unk4) {
            r->unk4 = r->unk0;
        }
        D_8005B830.unkB0 = 0x2B;
    }
}
