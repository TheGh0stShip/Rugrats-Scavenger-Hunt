typedef unsigned char u8;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

typedef struct Gfx_8000D0AC {
    u32 w0;
    u32 w1;
} Gfx_8000D0AC;

typedef struct SpritePart_8000D0AC {
    u16 x;
    u16 y;
    u32 image;
    u8 pad_08[4];
} SpritePart_8000D0AC;

typedef struct SpriteHeader_8000D0AC {
    u16 count;
    u8 pad_02[6];
    u16 format;
    u8 pad_0A;
    u8 width;
    u8 pad_0C;
    u8 height;
    u16 image_base;
    u16 palette;
    u8 pad_12[6];
    SpritePart_8000D0AC parts[1];
} SpriteHeader_8000D0AC;

extern Gfx_8000D0AC *D_8007784C;

#define WRITE_GFX(hi, lo)           \
    {                               \
        Gfx_8000D0AC *gfx;          \
        gfx = D_8007784C++;         \
        gfx->w0 = (hi);             \
        gfx->w1 = (lo);             \
    }

#define WRITE_RECT(part, width, height)                                              \
    WRITE_GFX(                                                                       \
        0xE4000000 | (((((part)->x + (width)) << 2) & 0xFFF) << 12) |               \
            (((part)->y + (height)) << 2 & 0xFFF),                                   \
        ((((part)->x << 2) & 0xFFF) << 12) | (((part)->y << 2) & 0xFFF)             \
    );                                                                                \
    WRITE_GFX(0xB4000000, 0);                                                        \
    WRITE_GFX(0xB3000000, 0x04000400)

void func_8000D0AC(SpriteHeader_8000D0AC *sprite) {
    u8 *base = (u8 *)sprite + sprite->image_base;
    u8 *palette;
    u8 width = sprite->width;
    s32 height = sprite->height;
    s32 i = 0;
    volatile SpritePart_8000D0AC *part;

    if (sprite->format == 8) {
        palette = (u8 *)sprite + sprite->palette;
    }

    WRITE_GFX(0xFCFFFFFF, 0xFFFCF87C);
    WRITE_GFX(0xBA001001, 0x00010000);
    WRITE_GFX(0xBA001102, 0);
    WRITE_GFX(0xBA001301, 0);
    WRITE_GFX(0xBB000001, 0x80008000);
    WRITE_GFX(0xBA000C02, 0);
    WRITE_GFX(0xBA000903, 0x00000C00);
    WRITE_GFX(0xBA000E02, 0);
    WRITE_GFX(0xB900031D, 0x0F0A4000);

    if (sprite->format == 8) {
        WRITE_GFX(0xBA000E02, 0x8000);
        WRITE_GFX(0xFD100000, (u32)palette);
        WRITE_GFX(0xE8000000, 0);
        WRITE_GFX(0xF5000100, 0x07000000);
        WRITE_GFX(0xE6000000, 0);
        WRITE_GFX(0xF0000000, 0x073FC000);
        WRITE_GFX(0xE7000000, 0);
    }
    WRITE_GFX(0xE6000000, 0);

    switch (sprite->format) {
    case 8: {
        s32 w;
        s32 h;
        u32 words;

        part = sprite->parts;
        if (i < sprite->count) {
            w = width;
            h = height & 0xFF;
            words = (u32)w >> 3;
            do {
            WRITE_GFX(0xFD500000, (u32)(base + part->image));
            WRITE_GFX(0xF5500000, 0x07094260);
            WRITE_GFX(0xE6000000, 0);
            {
            Gfx_8000D0AC *load;
            register Gfx_8000D0AC *div_load asm("$5");
            u32 load_word;
            s32 lrs;
            s32 dxt;

            load = D_8007784C++;
            load->w0 = 0xF3000000;
            div_load = load;
            dxt = 0x800;
            if (words != 0) {
                dxt = words + 0x7FF;
            }
            lrs = (((w * h) + 1) >> 1) - 1;
            if (lrs >= 0x800) {
                lrs = 0x7FF;
            }
            load_word = ((lrs & 0xFFF) << 12) | 0x07000000;
            if (words != 0) {
                div_load->w1 = load_word | (dxt / (s32)words);
            } else {
                load->w1 = load_word | dxt;
            }
            }
                i++;
            WRITE_GFX(0xE7000000, 0);
            WRITE_GFX(
                0xF5480000 | ((((u32)w + 7) >> 3) << 9),
                0x00094260
            );
            WRITE_GFX(
                0xF2000000,
                ((((w - 1) << 2) & 0xFFF) << 12) |
                    (((h - 1) << 2) & 0xFFF)
            );
                WRITE_RECT(part, w, h);
                part++;
            } while (i < sprite->count);
        }
        break;
    }
    case 16: {
        s32 w;
        s32 h;
        s32 size;

        part = sprite->parts;
        if (i < sprite->count) {
            w = width;
            h = height & 0xFF;
            size = w * h;
            do {
            WRITE_GFX(0xFD100000, (u32)(base + part->image));
            WRITE_GFX(0xF5100000, 0x07094260);
            WRITE_GFX(0xE6000000, 0);
            {
            Gfx_8000D0AC *load;
            u32 load_word;
            s32 lrs;
            s32 dxt;
            register s32 quarter asm("$10");
            register s32 quarter_input asm("$9");
            s32 divisor;
            u32 result;

            load = D_8007784C++;
            load->w0 = 0xF3000000;
            quarter_input = width;
            quarter = quarter_input;
            if (quarter_input < 0) {
                quarter = quarter_input + 3;
            }
            quarter >>= 2;
            dxt = 0x800;
            if (quarter > 0) {
                dxt = quarter + 0x7FF;
            }
            lrs = size - 1;
            if (lrs >= 0x800) {
                lrs = 0x7FF;
            }
            load_word = ((lrs & 0xFFF) << 12) | 0x07000000;
            divisor = w;
            if (divisor < 0) {
                divisor += 3;
            }
            divisor >>= 2;
            if (divisor <= 0) {
                result = dxt & 0xFFF;
            } else {
                result = (dxt / divisor) & 0xFFF;
            }
            load->w1 = load_word | result;
            }
                i++;
            WRITE_GFX(0xE7000000, 0);
            WRITE_GFX(
                0xF5100000 | ((((u32)w * 2 + 7) >> 3) << 9),
                0x00094260
            );
            WRITE_GFX(
                0xF2000000,
                ((((w - 1) << 2) & 0xFFF) << 12) |
                    (((h - 1) << 2) & 0xFFF)
            );
                WRITE_RECT(part, w, h);
                part++;
            } while (i < sprite->count);
        }
        break;
    }
    }
}
