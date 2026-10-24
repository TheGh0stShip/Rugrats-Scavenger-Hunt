/* Target: func_8001FC2C  VRAM 0x8001FC2C  ROM 0x2082C
 * Size: 0x308  Compiler: SN Systems GCC 2.8 (-O2) */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef float f32;

typedef struct { u32 w0; u32 w1; } Gfx;

#define GFX(pp, a, b) { Gfx *_g = (pp)++; _g->w0 = (a); _g->w1 = (b); }

struct Frame_8006D680 {
    Gfx *dl[8];
    u8 buf[8][0xE10];
};

extern Gfx *D_8007784C;
extern struct Frame_8006D680 *D_8006D680;
extern u32 D_8006D698[2];
extern void func_80047E80(void);

void func_8001FC2C(void)
{
    s32 i, j;
    struct Frame_8006D680 *f;

    GFX(D_8007784C, 0xE6000000, 0);
    GFX(D_8007784C, 0xE7000000, 0);
    GFX(D_8007784C, 0xBA001402, 0);
    GFX(D_8007784C, 0xFC11B223, 0xFF67FFFF);
    GFX(D_8007784C, 0xBA001001, 0x10000);
    GFX(D_8007784C, 0xBA001102, 0);
    GFX(D_8007784C, 0xBA001301, 0);
    GFX(D_8007784C, 0xBB000001, 0x80008000);
    GFX(D_8007784C, 0xBA000C02, 0x3000);
    GFX(D_8007784C, 0xBA000903, 0xC00);
    GFX(D_8007784C, 0xBA000E02, 0x8000);
    GFX(D_8007784C, 0xB900031D, 0x504240);
    GFX(D_8007784C, 0xFA000000, 0xFFFFFFFF);
    GFX(D_8007784C, 0xE7000000, 0);

    f = D_8006D680;
    for (i = 0; i < 2; i++, f++) {
        if (D_8006D698[i] != 0) {
            GFX(D_8007784C, 0xFD100000, D_8006D698[i] + 0x30);
            GFX(D_8007784C, 0xE8000000, 0);
            GFX(D_8007784C, 0xF5000100, 0x07000000);
            GFX(D_8007784C, 0xE6000000, 0);
            GFX(D_8007784C, 0xF0000000, 0x0703C000);
            GFX(D_8007784C, 0xE7000000, 0);
            for (j = 0; j < 8; j++) {
                GFX(D_8007784C, 0x06000000, (u32)f->buf[j]);
                GFX(f->dl[j], 0xB8000000, 0);
            }
        }
    }
    func_80047E80();
}
