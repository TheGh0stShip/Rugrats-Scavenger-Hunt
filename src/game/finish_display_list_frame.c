/* func_8003376C: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef struct { u32 w0; u32 w1; } Gfx;

typedef struct GfxPool_8009C3C0 { u8 data[0xE790]; } GfxPool_8009C3C0;
extern Gfx *D_8007784C;
extern Gfx *D_80077830;
extern u32 D_80052F00;
extern s32 D_80054358;
extern GfxPool_8009C3C0 D_8009C3C0[];
extern s32 D_80054838;
extern s32 D_80054824;
extern void func_80047E80(void);

void func_8003376C(void) {
    s32 used;
    s32 off;
    GfxPool_8009C3C0 *pool;

    off = 0xC180;

    { Gfx *_g = D_8007784C++; _g->w0 = 0x06000000; _g->w1 = D_80052F00 + off; }
    { Gfx *_g = D_80077830++; _g->w0 = 0xB8000000; _g->w1 = 0; }
    func_80047E80();
    used = ((u8 *)D_80077830 - off - (u8 *)(pool = &D_8009C3C0[D_80054358])) >> 3;
    if (D_80054838 < used) {
        D_80054838 = used;
    }
    D_80054824 = D_80054824 == 0;
}
