typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;
typedef struct { u32 w0, w1; } Gfx;

typedef struct DisplayFrame {
    Gfx *dl[8];
    u8 pad_20[0x70A0 - 0x20];
} DisplayFrame;

extern DisplayFrame *D_8006D680;
extern s32 D_8006D684;

void func_8001FB84(s32 r, u8 g, u8 b, u8 a)
{
    s32 i;
    Gfx **pp;

    i = 0;
    pp = D_8006D680[D_8006D684].dl;
    for (; i < 8; i++, pp++) {
        { Gfx *_g = (*pp)++; _g->w0 = 0xE7000000; _g->w1 = 0; }
        { Gfx *_g = (*pp)++; _g->w0 = 0xFA000000; _g->w1 = (r << 24) | (g << 16) | (b << 8) | a; }
    }
}
