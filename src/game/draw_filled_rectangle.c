/* func_8003318C: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef struct { u32 w0; u32 w1; } Gfx;

extern Gfx *D_80077830;

void func_8003318C(s32 x0, s32 y0, s32 x1, s32 y1, s32 r, s32 g, s32 b, s32 a) {
    { Gfx *_g = D_80077830++; _g->w0 = 0xE7000000; _g->w1 = 0; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xFA000000; _g->w1 = (((u32)r & 0xFF) << 24) | (((u32)g & 0xFF) << 16) | (((u32)b & 0xFF) << 8) | ((u32)a & 0xFF); }
    { Gfx *_g = D_80077830++; _g->w0 = 0xFCFFFFFF; _g->w1 = 0xFFFDF6FB; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xB900031D; _g->w1 = 0x00504240; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xF6000000 | ((x1 & 0x3FF) << 14) | ((y1 & 0x3FF) << 2);
      _g->w1 = ((x0 & 0x3FF) << 14) | ((y0 & 0x3FF) << 2); }
}
