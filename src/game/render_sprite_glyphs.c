typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct Gfx {
    u32 w0;
    u32 w1;
} Gfx;

typedef struct Glyph {
    u8 unk0;
    u8 width;
    u8 unk2;
    u8 height;
    u16 x;
    u16 y;
    u32 texture;
    u32 unkC;
} Glyph;

typedef struct Sprite {
    u16 count;
    u8 pad2[6];
    u16 type;
    u8 padA[4];
    u16 pixels;
    u16 palette;
    u8 pad12[6];
    Glyph glyphs[1];
} Sprite;

extern Gfx *D_80077830;

#define CMD(a, b)                                                            \
    do {                                                                     \
        Gfx *pkt = D_80077830++;                                             \
        pkt->w0 = (a);                                                       \
        pkt->w1 = (b);                                                       \
    } while (0)

void func_80032A20(Sprite *spr, s32 x, s32 y) {
    s32 i = 0;
    u8 *pixels = (u8 *)spr + spr->pixels;
    u8 *palette;
    Glyph *g;
    s8 w;
    s8 h;
    s32 px;
    s32 py;
    u32 texels;
    u32 line;
    u32 lrs;
    u32 dxt;

    if (spr->type == 8) {
        palette = (u8 *)spr + spr->palette;
    }

    CMD(0xFC11B223, 0xFF67FFFF);
    CMD(0xBA001402, 0);
    CMD(0xBA001001, 0x00010000);
    CMD(0xBA001102, 0);
    CMD(0xBA001301, 0);
    CMD(0xBB000001, 0x80008000);
    CMD(0xBA000C02, 0);
    CMD(0xBA000903, 0x00000C00);

    if (spr->type == 8) {
        CMD(0xBA000E02, 0x00008000);
        CMD(0xB900031D, 0x00504240);
        CMD(0xE6000000, 0);
        CMD(0xFD100000, (u32)palette);
        CMD(0xE8000000, 0);
        CMD(0xF5000100, 0x07000000);
        CMD(0xE6000000, 0);
        CMD(0xF0000000, 0x073FC000);
        CMD(0xE7000000, 0);

        for (i = 0, g = spr->glyphs; i < spr->count; i++, g++) {
            w = g->width;
            h = g->height;
            px = x + g->x;
            py = y + g->y;
            CMD(0xFD100000, (u32)(pixels + g->texture));
            CMD(0xF5100000, 0x07094260);
            CMD(0xE6000000, 0);
            texels = (u32)w * h;
            lrs = ((texels + 1) / 2) - 1;
            if (lrs >= 0x800) {
                lrs = 0x7FF;
            }
            line = w >> 3;
            dxt = line ? (line + 0x7FF) / line : 0x800;
            CMD(0xF3000000, 0x07000000 | ((lrs & 0xFFF) << 12) | dxt);
            CMD(0xE7000000, 0);
            CMD(0xF5080000 | (((w + 7) >> 3) << 9), 0x00094260);
            CMD(0xF2000000,
                (((w - 1) * 4 & 0xFFF) << 12) | ((h - 1) * 4 & 0xFFF));
            CMD(0xE4000000 | (((px + w) * 4 & 0xFFF) << 12) |
                                  ((py + h) * 4 & 0xFFF),
                ((px * 4 & 0xFFF) << 12) | (py * 4 & 0xFFF));
            CMD(0xB4000000, 0);
            CMD(0xB3000000, 0x04000400);
        }
    } else if (spr->type == 16) {
        CMD(0xBA000E02, 0x00008000);
        CMD(0xE6000000, 0);
        line = (s32)(u8)w / 4;
        dxt = line > 0 ? (0x7FF + line) / line : 0x800;
        texels = (u8)w * (u8)h;
        lrs = texels - 1;
        if (lrs >= 0x800) {
            lrs = 0x7FF;
        }

        for (i = 0, g = spr->glyphs; i < spr->count; i++, g++) {
            px = x + g->x;
            py = y + g->y;
            CMD(0xFD100000, (u32)(pixels + g->texture));
            CMD(0xF5100000 | (((((u8)w * 2) + 7) >> 3) << 9), 0x07094260);
            CMD(0xE6000000, 0);
            CMD(0xF3000000,
                0x07000000 | ((lrs & 0xFFF) << 12) | (dxt & 0xFFF));
            CMD(0xE7000000, 0);
            CMD(0xF5100000 | (((((u8)w * 2) + 7) >> 3) << 9), 0x00094260);
            CMD(0xF2000000,
                ((((u8)w - 1) * 4 & 0xFFF) << 12) |
                    (((u8)h - 1) * 4 & 0xFFF));
            CMD(0xE4000000 | (((px + (u8)w) * 4 & 0xFFF) << 12) |
                                  ((py + (u8)h) * 4 & 0xFFF),
                ((px * 4 & 0xFFF) << 12) | (py * 4 & 0xFFF));
            CMD(0xB4000000, 0);
            CMD(0xB3000000, 0x04000400);
        }
    }
}
