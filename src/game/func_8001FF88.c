typedef unsigned char u8;
typedef int s32;

struct Glyph_8006D688 {
    u8 width;
    u8 pad_01[7];
};

struct Font_8006D688 {
    u8 pad_00[0x14];
    s32 spacing;
    u8 pad_18[0x38];
    struct Glyph_8006D688 glyph[256];
};

extern struct Font_8006D688 *D_8006D688;

s32 func_8001FF88(u8 *str)
{
    struct Font_8006D688 *font = D_8006D688;
    s32 width = -font->spacing;

    while (*str != 0 && *str != '\n') {
        width += font->glyph[*str++].width + font->spacing;
    }
    return width;
}
