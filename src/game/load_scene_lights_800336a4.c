typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef struct { u32 w0; u32 w1; } Gfx;

typedef struct Light_800C3CE0 { u8 data[0x10]; } Light_800C3CE0;
extern Light_800C3CE0 D_800C3CE0[4];
extern Gfx *D_80077830;
extern s32 D_80054820;
extern void func_80046A80(Light_800C3CE0 *l);

void func_800336A4(void) {
    Light_800C3CE0 *l = D_800C3CE0;

    func_80046A80(l);
    D_80054820 = 0;
    { Gfx *_g = D_80077830++; _g->w0 = 0x039E0010; _g->w1 = (u32)&l[0]; }
    { Gfx *_g = D_80077830++; _g->w0 = 0x03980010; _g->w1 = (u32)&l[1]; }
    { Gfx *_g = D_80077830++; _g->w0 = 0x039A0010; _g->w1 = (u32)&l[2]; }
    { Gfx *_g = D_80077830++; _g->w0 = 0x039C0010; _g->w1 = (u32)&l[3]; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xB6000000; _g->w1 = 0x2000; }
}
