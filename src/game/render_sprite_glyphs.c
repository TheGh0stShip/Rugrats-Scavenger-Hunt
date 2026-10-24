/* Target: func_80032A20  VRAM 0x80032A20  ROM 0x33620
 * Size: 0x6F8  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: low */
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct { u32 w0; u32 w1; } Gfx;

typedef struct Glyph_80032A20 {
    u8 pad_0;
    u8 w;          /* 0x1 */
    u8 pad_2;
    u8 h;          /* 0x3 */
    u16 x;         /* 0x4 */
    u16 y;         /* 0x6 */
    u32 texOffset; /* 0x8 */
    u8 pad_C[0x4];
} Glyph_80032A20;

typedef struct Text_80032A20 {
    u16 count;        /* 0x00 */
    u8 pad_02[0x6];
    u16 fmt;          /* 0x08: 8 = CI4 + TLUT, 0x10 = 16-bit */
    u8 pad_0A[0x4];
    u16 texBase;      /* 0x0E */
    u16 palOffset;    /* 0x10 */
    u8 pad_12[0x6];
    Glyph_80032A20 glyphs[1]; /* 0x18 */
} Text_80032A20;

extern Gfx *D_80077830;

#define GFXW(a, b) { Gfx *g_ = D_80077830++; g_->w0 = (a); g_->w1 = (b); }

void func_80032A20(Text_80032A20 *t, s32 x, s32 y) {
    u8 *tex;
    u8 *pal;
    s32 i;
    u8 w, h;
    s32 x0, y0;
    s32 wb, dxtNum, lrs, q;
    Glyph_80032A20 *g;

    tex = (u8 *)t + t->texBase;
    i = 0;
    if (t->fmt == 8) {
        pal = (u8 *)t + t->palOffset;
    }
    GFXW(0xFC11B223, 0xFF67FFFF);
    GFXW(0xBA001402, 0);
    GFXW(0xBA001001, 0x10000);
    GFXW(0xBA001102, 0);
    GFXW(0xBA001301, 0);
    GFXW(0xBB000001, 0x80008000);
    GFXW(0xBA000C02, 0);
    GFXW(0xBA000903, 0xC00);

    if (t->fmt == 8) {
        GFXW(0xBA000E02, 0x8000);
        GFXW(0xB900031D, 0x00504240);
        GFXW(0xE6000000, 0);
        GFXW(0xFD100000, (u32)pal);
        GFXW(0xE8000000, 0);
        GFXW(0xF5000100, 0x07000000);
        GFXW(0xE6000000, 0);
        GFXW(0xF0000000, 0x073FC000);
        GFXW(0xE7000000, 0);
        for (; i < t->count; i++) {
            g = &t->glyphs[i];
            w = g->w;
            h = g->h;
            x0 = x + g->x;
            y0 = y + g->y;
            GFXW(0xFD100000, (u32)(tex + g->texOffset));
            GFXW(0xF5100000, 0x07094260);
            GFXW(0xE6000000, 0);
            {
                Gfx *gp = D_80077830++;
                gp->w0 = 0xF3000000;
                wb = w >> 3;
                dxtNum = 0x800;
                if (wb != 0) {
                    dxtNum = wb + 0x7FF;
                }
                lrs = ((s32)(h * w + 1) >> 1) - 1;
                if (lrs >= 0x800) {
                    lrs = 0x7FF;
                }
                if (wb != 0) {
                    gp->w1 = ((lrs & 0xFFF) << 12) | 0x07000000 | (dxtNum / wb);
                } else {
                    gp->w1 = ((lrs & 0xFFF) << 12) | 0x07000000 | dxtNum;
                }
            }
            GFXW(0xE7000000, 0);
            GFXW(0xF5080000 | ((((u32)w + 7) >> 3) << 9), 0x00094260);
            GFXW(0xF2000000, ((((w - 1) << 2) & 0xFFF) << 12) | (((h - 1) << 2) & 0xFFF));
            GFXW(0xE4000000 | ((((x0 + w) << 2) & 0xFFF) << 12) | (((y0 + h) << 2) & 0xFFF),
                 (((x0 << 2) & 0xFFF) << 12) | ((y0 << 2) & 0xFFF));
            GFXW(0xB4000000, 0);
            GFXW(0xB3000000, 0x04000400);
        }
    } else if (t->fmt == 0x10) {
        GFXW(0xBA000E02, 0x8000);
        GFXW(0xE6000000, 0);
        for (; i < t->count; i++) {
            g = &t->glyphs[i];
            x0 = x + g->x;
            y0 = y + g->y;
            GFXW(0xFD100000, (u32)(tex + g->texOffset));
            GFXW(0xF5100000, 0x07094260);
            GFXW(0xE6000000, 0);
            {
                Gfx *gp = D_80077830++;
                gp->w0 = 0xF3000000;
                q = (s32)(w & 0xFF) / 4;
                dxtNum = 0x800;
                if (q > 0) {
                    dxtNum = q + 0x7FF;
                }
                lrs = (s32)((w & 0xFF) * (h & 0xFF)) - 1;
                if (lrs >= 0x800) {
                    lrs = 0x7FF;
                }
                if (q > 0) {
                    gp->w1 = ((lrs & 0xFFF) << 12) | 0x07000000 | ((dxtNum / q) & 0xFFF);
                } else {
                    gp->w1 = ((lrs & 0xFFF) << 12) | 0x07000000 | (dxtNum & 0xFFF);
                }
            }
            GFXW(0xE7000000, 0);
            GFXW(0xF5100000 | (((((u32)w & 0xFF) * 2 + 7) >> 3) << 9), 0x00094260);
            GFXW(0xF2000000, (((((w & 0xFF) - 1) << 2) & 0xFFF) << 12) | ((((h & 0xFF) - 1) << 2) & 0xFFF));
            GFXW(0xE4000000 | ((((x0 + (w & 0xFF)) << 2) & 0xFFF) << 12) | (((y0 + (h & 0xFF)) << 2) & 0xFFF),
                 (((x0 << 2) & 0xFFF) << 12) | ((y0 << 2) & 0xFFF));
            GFXW(0xB4000000, 0);
            GFXW(0xB3000000, 0x04000400);
        }
    }
}
