/* Target: func_80012DBC  VRAM 0x80012DBC  ROM 0x139BC
 * Size: 0x1E8  Compiler: SN GCC 2.8 (-O2)
 * Confidence: medium
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

struct Fade_80058C18 {
    s32 unk0;
    f32 unk4;
    f32 unk8;
    s32 unkC;
    s32 unk10;
};

extern Gfx *D_8007784C;
extern void (*D_80058C10)(void);
extern struct Fade_80058C18 D_80058C18;
extern s32 D_8005435C;

#define GFX_W(hi, lo)              \
    {                              \
        Gfx *_g = D_8007784C++;    \
        _g->w0 = (hi);             \
        _g->w1 = (lo);             \
    }

s32 func_80012DBC(void) {
    struct Fade_80058C18 *f = &D_80058C18;

    if (f->unk10 != 0) {
        return -1;
    }
    GFX_W(0xE7000000, 0);
    GFX_W(0xB900031D, 0x00504240);
    GFX_W(0xFCFFFFFF, 0xFFFDF6FB);
    GFX_W(0xFA000000, (s32)f->unk4 & 0xFF);
    GFX_W(0xF68003C0, 0);
    GFX_W(0xE7000000, 0);

    if (f->unkC == 1) {
        D_8005435C = 4;
        f->unk4 += f->unk8;
        if (f->unk4 >= 255.0f) {
            f->unk4 = 255.0f;
        }
    } else {
        f->unk4 -= f->unk8;
        if (f->unk4 <= 0.0f) {
            f->unk4 = 0.0f;
        }
    }

    if (D_80058C18.unk4 == 255.0f || D_80058C18.unk4 == 0.0f) {
        if (D_80058C18.unkC == 1) {
            D_8005435C = 5;
            D_80058C18.unk10 = 1;
            if (D_80058C10 != 0) {
                D_80058C10();
                D_80058C10 = 0;
            }
        } else if (D_80058C18.unkC == 0) {
            D_8005435C = 1;
            D_80058C18.unkC = -1;
            D_80058C18.unk10 = 1;
            if (D_80058C10 != 0) {
                D_80058C10();
                D_80058C10 = 0;
            }
        }
        return 1;
    }
    D_80058C18.unk10 = 0;
    return 0;
}
